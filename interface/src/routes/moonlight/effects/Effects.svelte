<!--
   @title     MoonLight
   @file      Effects.svelte
   @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
   @Authors   https://github.com/MoonModules/MoonLight/commits/main
   @Doc       https://moonmodules.org/MoonLight/moonlight/effects/
   @Copyright © 2025 Github MoonLight Commit Authors
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
-->

<svelte:options immutable={true} />

<script lang="ts">
	import { openModal, closeModal } from 'svelte-modals';
	import { slide } from 'svelte/transition';
	import { cubicOut } from 'svelte/easing';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import DragDropList, { VerticalDropZone, reorder, type DropEvent } from 'svelte-dnd-list';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import ConfirmDialog from '$lib/components/ConfirmDialog.svelte';
	import Spinner from '$lib/components/Spinner.svelte';
	import EffectsIcon from '~icons/tabler/torii';
	import Add from '~icons/tabler/circle-plus';
	import Edit from '~icons/tabler/pencil';
	import Delete from '~icons/tabler/trash';
	import Cancel from '~icons/tabler/x';
	import Check from '~icons/tabler/check';
	import InfoDialog from '$lib/components/InfoDialog.svelte';
	import type { EffectsState } from '$lib/types/models';
	import Select from '$lib/components/Select.svelte';
	import { onMount, onDestroy } from 'svelte';
	import { socket } from '$lib/stores/socket';
	import type { StarState } from '$lib/types/models';
	import FileEdit from '$lib/components/FileEdit.svelte';

	let effectsState: EffectsState;
	let dataLoaded = false;
	let starState: StarState;
	let starLoaded = false;
	let effectsList: EffectsState[] = [];
	let editableEffect: EffectsState = {
		name: '',
		effect: -1,
		projection: -1,
		nodes: []
	};

	let newItem: boolean = true;
	let showEditor: boolean = false;
	let formField: any;
	let formErrors = {
		name: false
	};
	let formErrorFilename = false;

	let path = "";

	async function getState() {
		try {
			const response = await fetch('/rest/effectsState', {
				method: 'GET',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			console.log("getState Effects.effectsState");
			handleEffectsState(await response.json());
		} catch (error) {
			console.error('Error:', error);
		}

		try {
			const response = await fetch('/rest/starState', {
				method: 'GET',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			console.log("getState Effects.starState");
			handleStarState(await response.json());
		} catch (error) {
			console.error('Error:', error);
		}
		path = starState.effects[effectsState.effect]
		return effectsState;
	}

	async function postEffectsState(data: EffectsState) {
		try {
			const response = await fetch('/rest/effectsState', {
				method: 'POST',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify(data)
			});
			if (response.status == 200) {
				notifications.success('Settings updated.', 3000);
				effectsState = await response.json();
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}

	function validateItem() {
		if (effectsState.name.length < 3 || effectsState.name.length > 32) {
			formErrorFilename = true;
		} else {
			formErrorFilename = false;
			// Update global effectsState object
			effectsState.nodes = effectsList;
			// Post to REST API
			postEffectsState(effectsState);
			console.log(effectsState);
		}
	}

	function validateForm() {
		let valid = true;

		// Validate Name
		if (editableEffect.name.length < 3 || editableEffect.name.length > 32) {
			valid = false;
			formErrors.name = true;
		} else {
			formErrors.name = false;
		}

		// Submit JSON to REST API
		if (valid) {
			if (newItem) {
				effectsList.push(editableEffect);
			} else {
				effectsList.splice(effectsList.indexOf(editableEffect), 1, editableEffect);
			}
			addItem();
			effectsList = [...effectsList]; //Trigger reactivity
			showEditor = false;
		}
	}

	function addItem() {
		newItem = true;
		editableEffect = {
			name: '',
			effect: -1,
			projection: -1,
			nodes: []
		};
	}

	function handleEdit(index: number) {
		newItem = false;
		showEditor = true;
		editableEffect = effectsList[index];
	}

	function confirmDelete(index: number) {
		openModal(ConfirmDialog, {
			title: 'Delete item',
			message: 'Are you sure you want to delete ' + effectsList[index].name + '?',
			labels: {
				cancel: { label: 'Cancel', icon: Cancel },
				confirm: { label: 'Delete', icon: Delete }
			},
			onConfirm: () => {
				// Check if item is currently been edited and delete as well
				if (effectsList[index].name === editableEffect.name) {
					addItem();
				}
				// Remove item from array
				effectsList.splice(index, 1);
				effectsList = [...effectsList]; //Trigger reactivity
				showEditor = false;
				closeModal();
			}
		});
	}

	function checkItemList() {
		if (effectsList.length >= 10) {
			openModal(InfoDialog, {
				title: 'Reached Maximum items',
				message:
					'You have reached the maximum number of items. Please delete one to add another.',
				dismiss: { label: 'OK', icon: Check },
				onDismiss: () => {
					closeModal();
				}
			});
			return false;
		} else {
			return true;
		}
	}

	function onDrop({ detail: { from, to } }: CustomEvent<DropEvent>) {
		if (!to || from === to) {
			return;
		}

		effectsList = reorder(effectsList, from.index, to.index);
		console.log(effectsList);
	}

	const handleEffectsState = (data: EffectsState) => {
		effectsState = data;
		if (effectsState.nodes) //sometimes error null...
			effectsList = effectsState.nodes;
		console.log("Effects.handleEffectsState", data);
		dataLoaded = true;
	};
	const handleStarState = (data: StarState) => {
		console.log("Effects.handleStarState", data);
		starState = data;
		starLoaded = true;
	};

	onMount(() => {
		console.log("onMount Effects");
		socket.on("effects", handleEffectsState);
		// socket.on("stars", handleStarState); //no updates of effect and projection expected
		// getState(); //done in settingscard
	});

	onDestroy(() => {
		console.log("onDestroy Effects");
		socket.off("effects", handleEffectsState);
		// socket.off("stars", handleStarState);
	});

	function sendSocket() {
		console.log("sendSocket Effects.effects", effectsState);
		path = starState.effects[effectsState.effect]
		if (dataLoaded) 
			socket.sendEvent('effects', effectsState)
	}

</script>

<SettingsCard collapsible={false}>
	<EffectsIcon slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Effects</span>
	<div class="w-full overflow-x-auto">
	</div>

	{#if !$page.data.features.security || $user.admin}
		<div class="bg-base-200 shadow-lg relative grid w-full max-w-2xl self-center overflow-hidden">
			{#await getState()}
				<Spinner />
			{:then nothing}
				<div>
					<Select label="Effect" bind:value={effectsState.effect} onChange={sendSocket}>
						{#each starState.effects as effect, i}
							<option value={i}>
								{effect}
							</option>
						{/each}
					</Select>
					<FileEdit
						path = {path}
						showEditor = {false}
					/>
					<Select label="Projection" bind:value={effectsState.projection} onChange={sendSocket}>
						{#each starState.projections as projection, i}
							<option value={i}>
								{projection}
							</option>
						{/each}
					</Select>
				</div>

				<div class="h-16 flex w-full items-center justify-between space-x-3 p-0 text-xl font-medium">
					Effects and Projections
				</div>
				<div class="relative w-full overflow-visible">
					<button
						class="btn btn-primary text-primary-content btn-md absolute -top-14 right-16"
						on:click={() => {
							if (checkItemList()) {
								addItem();
								showEditor = true;
							}
						}}
					>
						<Add class="h-6 w-6" /></button
					>

					<div
						class="overflow-x-auto space-y-1"
						transition:slide|local={{ duration: 300, easing: cubicOut }}
					>
						<DragDropList
							id="effects"
							type={VerticalDropZone}
							itemSize={60}
							itemCount={effectsList.length}
							on:drop={onDrop}
							let:index
						>
							<!-- svelte-ignore a11y-click-events-have-key-events -->
							<div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2">
								<div class="mask mask-hexagon bg-primary h-auto w-10 shrink-0">
									<EffectsIcon class="text-primary-content h-auto w-full scale-75" />
								</div>
								<div>
									<div class="font-bold">{effectsList[index].name}</div>
								</div>
								{#if !$page.data.features.security || $user.admin}
									<div class="flex-grow" />
									<div class="space-x-0 px-0 mx-0">
										<button
											class="btn btn-ghost btn-sm"
											on:click={() => {
												handleEdit(index);
											}}
										>
											<Edit class="h-6 w-6" /></button
										>
										<button
											class="btn btn-ghost btn-sm"
											on:click={() => {
												confirmDelete(index);
											}}
										>
											<Delete class="text-error h-6 w-6" />
										</button>
									</div>
								{/if}
							</div>
						</DragDropList>
					</div>
				</div>

				<div
					class="flex flex-col gap-2 p-0"
					transition:slide|local={{ duration: 300, easing: cubicOut }}
				>
					<form
						class=""
						on:submit|preventDefault={validateForm}
						novalidate
						bind:this={formField}
					>

						{#if showEditor}
							<div class="divider my-0" />
							<div class="h-16 flex w-full items-center justify-between space-x-3 p-0 text-xl font-medium">
								Edit
							</div>
							<div
								class="grid w-full grid-cols-1 content-center gap-x-4 px-4 sm:grid-cols-2"
								transition:slide|local={{ duration: 300, easing: cubicOut }}
							>
								<div>
									<label class="label" for="name">
										<span class="label-text text-md">Name</span>
									</label>
									<input
										type="text"
										class="input input-bordered invalid:border-error w-full invalid:border-2 {formErrors.name
											? 'border-error border-2'
											: ''}"
										bind:value={editableEffect.name}
										id="name"
										min="2"
										max="32"
										required
									/>
									<label class="label" for="name">
										<span class="label-text-alt text-error {formErrors.name ? '' : 'hidden'}"
											>Name must be between 3 and 32 characters long</span
										>
									</label>
								</div>
								<div>
									<Select label="Source" bind:value={editableEffect.effect} onChange={()=>{}}>
										{#each starState.projections as projection, i}
											<option value={i}>
												{projection}
											</option>
										{/each}
									</Select>
								</div>
							</div>
						{/if}

						<div class="divider mb-2 mt-0" />
						<div class="mx-4 mb-4 flex flex-wrap justify-end gap-2">
							<button class="btn btn-primary" type="submit" disabled={!showEditor}
								>{newItem ? 'Add' : 'Update'}</button
							>
							<button class="btn btn-primary" type="button" on:click={validateItem}
								>Apply</button
							>
						</div>
					</form>
				</div>
			{/await}
		</div>
	{/if}
</SettingsCard>
