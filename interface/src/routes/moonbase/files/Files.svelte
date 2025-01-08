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
	import FilesIcon from '~icons/tabler/files';
	import Add from '~icons/tabler/circle-plus';
	import Edit from '~icons/tabler/pencil';
	import Delete from '~icons/tabler/trash';
	import Cancel from '~icons/tabler/x';
	import Check from '~icons/tabler/check';
	import InfoDialog from '$lib/components/InfoDialog.svelte';
	import type { FilesState } from '$lib/types/models';
	import Text from '$lib/components/Text.svelte';
	import Textarea from '$lib/components/Textarea.svelte';

	let itemsState: FilesState;
	let itemsList: FilesState[] = [];
	let editableItem: FilesState = {
		name: '',
		path: '',
		size: 0,
		time: 0,
		contents: 'wait',
		files: []
	};

	let newItem: boolean = true;
	let showEditor: boolean = false;
	let formField: any;
	let formErrors = {
		name: false
	};
	let formErrorFilename = false;

	async function getState() {
		try {
			const response = await fetch('/rest/filesState', {
				method: 'GET',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			itemsState = await response.json();
			itemsState = itemsState.files[0]; //temp, only show the first folder (which is config)
			// console.log("itemsState", itemsState);
		} catch (error) {
			console.error('Error:', error);
		}
		itemsList = itemsState.files;
		return itemsState;
	}

	async function postSettings(data: FilesState) {
		try {
			const response = await fetch('/rest/filesState', {
				method: 'POST',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify(data)
			});
			if (response.status == 200) {
				notifications.success('Settings updated.', 3000);
				itemsState = await response.json();
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}

	function validateItem() {
		if (itemsState.name.length < 3 || itemsState.name.length > 32) {
			formErrorFilename = true;
		} else {
			formErrorFilename = false;
			// Update global itemsState object
			itemsState.files = itemsList;
			// Post to REST API
			postSettings(itemsState);
			console.log(itemsState);
		}
	}

	function validateForm() {
		let valid = true;

		// Validate Name
		if (editableItem.name.length < 3 || editableItem.name.length > 32) {
			valid = false;
			formErrors.name = true;
		} else {
			formErrors.name = false;
		}

		// Submit JSON to REST API
		if (valid) {
			if (newItem) {
				itemsList.push(editableItem);
			} else {
				itemsList.splice(itemsList.indexOf(editableItem), 1, editableItem);
			}
			addItem();
			itemsList = [...itemsList]; //Trigger reactivity
			showEditor = false;
		}
	}

	function addItem() {
		newItem = true;
		editableItem = {
			name: '',
			path: '',
			size: 0,
			time: 0,
			contents: 'add',
			files: []
		};
	}

	async function handleEdit(index: number) {
		newItem = false;
		showEditor = true;
		editableItem = itemsList[index];
		try {
			const response = await fetch('/rest/file/config/' + editableItem.name, {
					method: 'GET',
				headers: {'Content-Type': 'text/plain'}
			});
			editableItem.contents = await response.text();
			console.log("handleEdit", editableItem.contents)
		} catch (error) {
			console.error('Error:', error);
		}
	}

	function confirmDelete(index: number) {
		openModal(ConfirmDialog, {
			title: 'Delete item',
			message: 'Are you sure you want to delete ' + itemsList[index].name + '?',
			labels: {
				cancel: { label: 'Cancel', icon: Cancel },
				confirm: { label: 'Delete', icon: Delete }
			},
			onConfirm: () => {
				// Check if item is currently been edited and delete as well
				if (itemsList[index].name === editableItem.name) {
					addItem();
				}
				// Remove item from array
				itemsList.splice(index, 1);
				itemsList = [...itemsList]; //Trigger reactivity
				showEditor = false;
				closeModal();
			}
		});
	}

	function checkItemList() {
		if (itemsList.length >= 10) {
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

		itemsList = reorder(itemsList, from.index, to.index);
		console.log(itemsList);
	}
</script>

<SettingsCard collapsible={false}>
	<FilesIcon slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Files info</span>
	<div class="w-full overflow-x-auto">
	</div>

	{#if !$page.data.features.security || $user.admin}
		<div class="bg-base-200 shadow-lg relative grid w-full max-w-2xl self-center overflow-hidden">
			<div class="h-16 flex w-full items-center justify-between space-x-3 p-0 text-xl font-medium">
				Files (/config)
			</div>
			{#await getState()}
				<Spinner />
			{:then nothing}
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
							id="files"
							type={VerticalDropZone}
							itemSize={60}
							itemCount={itemsList.length}
							on:drop={onDrop}
							let:index
						>
							<!-- svelte-ignore a11y-click-events-have-key-events -->
							<div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2">
								<div class="mask mask-hexagon bg-primary h-auto w-10 shrink-0">
									<FilesIcon class="text-primary-content h-auto w-full scale-75" />
								</div>
								<div>
									<div class="font-bold">{itemsList[index].name}</div>
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

								<div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2">
									<div class="mask mask-hexagon bg-primary h-auto w-10 flex-none">
										<FilesIcon class="text-primary-content h-auto w-full scale-75" />
									</div>
									<div>
										<div class="font-bold">File</div>
										<div class="text-sm opacity-75">
											{editableItem.path} {editableItem.size} 
										</div>
									</div>
								</div>
			
								<div>
									<Textarea 
										label="Contents" 
										bind:value={editableItem.contents} 
									></Textarea>
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
