<svelte:options immutable={true} />

<script lang="ts">
	import { openModal, closeModal } from 'svelte-modals';
	import { slide } from 'svelte/transition';
	import { cubicOut } from 'svelte/easing';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import ConfirmDialog from '$lib/components/ConfirmDialog.svelte';
	import Spinner from '$lib/components/Spinner.svelte';
	import FilesIcon from '~icons/tabler/files';
	import FileIcon from '~icons/tabler/file';
	import FolderIcon from '~icons/tabler/folder';
	import Add from '~icons/tabler/circle-plus';
	import Edit from '~icons/tabler/pencil';
	import Delete from '~icons/tabler/trash';
	import Cancel from '~icons/tabler/x';
	import type { FilesState } from '$lib/types/models';
	import Text from '$lib/components/Text.svelte';
	import Textarea from '$lib/components/Textarea.svelte';
	import File from '$lib/components/File.svelte';
	import { onMount, onDestroy } from 'svelte';
	import { socket } from '$lib/stores/socket';
	import { tick } from 'svelte';

	let filesState: FilesState;
	let folderList: FilesState[] = [];
	let editableFile: FilesState = {
		name: '',
		path: '',
		isFile: true,
		size: 0,
		time: 0,
		contents: '',
		files: []
	};
	let breadCrumbs:string[] = [];
	let breadCrumbsString:string = ""; //as breadCrumbs.join("/") is not reactive for some reason

	let newItem: boolean = true;
	let showEditor: boolean = false;
	let formField: any;
	let formErrors = {
		name: false
	};

	const cookieValue = getCookie('breadCrumbs');
	console.log("cookie", cookieValue);
	if (cookieValue) {
		breadCrumbs = JSON.parse(cookieValue);
		breadCrumbsString = breadCrumbs.join("/");
	}

	async function getState() {
		try {
			const response = await fetch('/rest/filesState', {
				method: 'GET',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			filesState = await response.json();
			// console.log("filesState", filesState);
		} catch (error) {
			console.error('Error:', error);
		}
		folderListFromBreadCrumbs();
		return filesState;
	}

	async function postFilesState(data: any) {
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
				filesState = await response.json();
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}

	function validateForm() {
		console.log("validateForm", editableFile.isFile)
		let valid = true;

		// Validate Name
		if (editableFile.name.length < 3 || editableFile.name.length > 32) {
			valid = false;
			formErrors.name = true;
		} else {
			formErrors.name = false;
		}

		// Submit JSON to REST API
		if (valid) {
			if (newItem) {
				folderList.push(editableFile);
				//order by name ...

				//send newfile or folder to server

				editableFile.path = "/" + breadCrumbs.join("/") + "/" + editableFile.name;
				let response:any = {};
				response.news = [];
				response.news.push(editableFile);
				console.log("new item", response)
				//send the new itemstate to server
				postFilesState(response);

			} else {
				console.log("update item", editableFile)
				folderList.splice(folderList.indexOf(editableFile), 1, editableFile);

				let response:any = {};
				response.updates = [];
				response.updates.push(editableFile);
				postFilesState(response);
			}
			showEditor = false;
		}
	}

	function addFile() {
		console.log("addFile")
		newItem = true;
		editableFile = {
			name: '',
			path: '',
			isFile: true,
			size: 0,
			time: 0,
			contents: '',
			files: []
		};
	}
	function addFolder() {
		console.log("addFolder")
		newItem = true;
		editableFile = {
			name: '',
			path: '',
			isFile: false,
			size: 0,
			time: 0,
			contents: '',
			files: []
		};
	}

	function folderListFromBreadCrumbs() {
		folderList = filesState.files;
		for (let indexF = 0; indexF < breadCrumbs.length; indexF++) {
			let found = false;
			for (let indexI = 0; indexI < folderList.length; indexI++) {
				if (folderList[indexI].name === breadCrumbs[indexF]) {
					console.log("handleEdit parent", folderList[indexI], breadCrumbs[indexF])
					folderList = [folderList[indexI], ...folderList[indexI].files];
					found = true;
				}
			}
			if (!found) { //e.g. old coookie, reset
				breadCrumbs = [];
				breadCrumbsString = "";
				folderList = filesState.files;
				return;
			}
		}
		// console.log("folderListFromBreadCrumbs", filesState, breadCrumbs, folderList)
	}

	async function handleEdit(index: number) {
		newItem = false;
		editableFile = folderList[index];

		if (breadCrumbs.length > 0 && editableFile.name === breadCrumbs[breadCrumbs.length-1]) { //if parent folder
			breadCrumbs.pop(); //remove last folder
			breadCrumbsString = breadCrumbs.join("/");
			folderListFromBreadCrumbs();

			setCookie('breadCrumbs', JSON.stringify(breadCrumbs), 7);
			showEditor = false;
			console.log("handleEdit parent", folderList, breadCrumbs)
		} else if (editableFile.isFile) { //if file
			try {
				const response = await fetch('/rest/file/' + editableFile.path, {
						method: 'GET',
					headers: {'Content-Type': 'text/plain'}
				});
				editableFile.contents = await response.text();
			} catch (error) {
				console.error('Error:', error);
			}
			console.log("handleEdit file", editableFile.contents)

			showEditor = false; await tick(); showEditor = true; //Trigger reactivity
		} else { //if folder, go to folder
			breadCrumbs.push(editableFile.name);
			breadCrumbsString = breadCrumbs.join("/");
			setCookie('breadCrumbs', JSON.stringify(breadCrumbs), 7);
			// folderList = [folderList[index], ...editableFile.files];
			folderListFromBreadCrumbs();
			// showEditor = true; await tick(); //wait for reactivity, not needed here
			showEditor = false;

			console.log("handleEdit go to folder", folderList, breadCrumbs)
		}
	}

	function confirmDelete(index: number) {
		openModal(ConfirmDialog, {
			title: 'Delete item',
			message: 'Are you sure you want to delete ' + folderList[index].name + '?',
			labels: {
				cancel: { label: 'Cancel', icon: Cancel },
				confirm: { label: 'Delete', icon: Delete }
			},
			onConfirm: () => {
				// Check if item is currently been edited and delete as well
				if (folderList[index].name === editableFile.name) {
					// addFile();
				}

				//update filesState
				let response:any = {};
				response.deletes = [];
				response.deletes.push(folderList[index]);
				console.log("confirmDelete", response)
				//send the new itemstate to server
				postFilesState(response);

				showEditor = false;
				closeModal();
			}
		});
	}

	function checkItemList() {
		return true;
	}

	function uploadFile(event: any) {
		let fileNode = event.target;
        let file = fileNode.files[0]; // the first file uploaded (multiple files not supported yet)
		// console.log("uploadFile", event, file)
		if (file) {
			// let fileContents: string | ArrayBuffer | null = null;

            const reader = new FileReader();
			reader.onload = async (e) => {
				const contents = e.target?.result;
				editableFile.name = file.name;
				editableFile.contents = typeof contents === 'string' ? contents : '';			

				showEditor = false; await tick(); showEditor = true; //Trigger reactivity (folderList = [...folderList]; is not doing it)
				console.log("uploadFileWithText", editableFile.contents)
			};
            reader.readAsText(file);
        }
	}

	const handleFilesState = (data: FilesState) => {
		console.log("socket update received");
		filesState = data;
		folderListFromBreadCrumbs();
		// dataLoaded = true;
	};

	onMount(() => {
		socket.on('files', handleFilesState);
		// getState(); //done in settingscard
	});

	onDestroy(() => socket.off('files', handleFilesState));

	//uitility function...
	function setCookie(name: string, value: string, days: number) {
		let expires = "";
		if (days) {
			const date = new Date();
			date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
			expires = "; expires=" + date.toUTCString();
		}
		document.cookie = name + "=" + (value || "") + expires + "; path=/";
	}

	//uitility funtcion...
	function getCookie(name: string) {
		const nameEQ = name + "=";
		const ca = document.cookie.split(';');
		for (let i = 0; i < ca.length; i++) {
			let c = ca[i];
			while (c.charAt(0) == ' ') c = c.substring(1, c.length);
			if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length, c.length);
		}
		return null;
	}

	//uitility function...
	function eraseCookie(name: string) {
		document.cookie = name + '=; Max-Age=-99999999;';
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
				Files /{breadCrumbsString}
			</div>
			{#await getState()}
				<Spinner />
			{:then nothing}
				<div class="relative w-full overflow-visible">
					<button
						class="btn btn-primary text-primary-content btn-md absolute -top-14 right-16"
						on:click={() => {
							if (checkItemList()) {
								addFile();
								showEditor = true;
							}
						}}
					>
						<Add class="h-6 w-6" /></button
					>
					<button
						class="btn btn-primary text-primary-content btn-md absolute -top-14 right-1"
						on:click={() => {
							if (checkItemList()) {
								addFolder();
								showEditor = true;
							}
						}}
					>
						<Add class="h-6 w-6" /></button
					>
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
								{newItem ? 'Add ' + (editableFile.isFile?"file":"folder") : 'Edit ' + editableFile.name}
							</div>

							<div>
								<Text
									label="Name" 
									bind:value={editableFile.name} 
								></Text>
								<label class="label" for="name">
									<span class="label-text-alt text-error {formErrors.name ? '' : 'hidden'}"
										>Name must be between 3 and 32 characters long</span
									>
								</label>
							</div>
							{#if editableFile.isFile}
								<div>
									<Textarea 
										label="Contents" 
										bind:value={editableFile.contents} 
										onChange={(event) => {
											editableFile.contents = event.target.value;
										}}
								></Textarea>
								</div>
								{#if newItem}
									<div>
										<File 
											label="Upload" 
											onChange={(event) => {
												uploadFile(event);
											}}
										></File>
									</div>
								{/if}
							{/if}
							<div class="divider mb-2 mt-0" />
							<div class="mx-4 mb-4 flex flex-wrap justify-end gap-2">
								<button class="btn btn-primary" type="submit" disabled={!showEditor}
									>Save</button
								>
							</div>
						{/if}

					</form>
				</div>

				<div
					class="overflow-x-auto space-y-1"
					transition:slide|local={{ duration: 300, easing: cubicOut }}
				>
					{#each folderList as item, index}

						<!-- svelte-ignore a11y-click-events-have-key-events -->
						<div class="rounded-box bg-base-100 flex items-center space-x-3 px-4 py-2">
							<div class="mask mask-hexagon bg-primary h-auto w-10 shrink-0">
								{#if item.isFile}
									<FileIcon class="text-primary-content h-auto w-full scale-75" />
								{:else}
									<FolderIcon class="text-primary-content h-auto w-full scale-75" />
								{/if}
							</div>
							<div>
								{#if breadCrumbs.length > 0 && item.name === breadCrumbs[breadCrumbs.length-1]}
									<div>..</div>
								{:else}
									<div class="font-bold">{item.name}</div>
									{#if item.isFile}
										<div>{item.size/1000} kb {new Intl.DateTimeFormat('en-GB', {
											dateStyle: 'short',
											timeStyle: 'short',
											timeZone: 'UTC'
										}).format(item.time)}
										</div>
									{:else}
										<div>{item.files.length} files/folders</div>
									{/if}
								{/if}
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
									{#if !(breadCrumbs.length > 0 && item.name === breadCrumbs[breadCrumbs.length-1])}
										<button
											class="btn btn-ghost btn-sm"
											on:click={() => {
												confirmDelete(index);
											}}
											disabled={item.files && item.files.length>0}
										>
											<Delete class="text-error h-6 w-6" />
										</button>
									{:else}
										&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
									{/if}
								</div>
							{/if}
						</div>
					{/each}
				</div>
			{/await}
		</div>
	{/if}
</SettingsCard>
