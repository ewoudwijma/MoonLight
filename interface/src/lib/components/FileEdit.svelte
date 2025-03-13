<!--
   @title     MoonLight
   @file      FileEdit.svelte
   @repo      https://github.com/MoonModules/MoonLight, submit changes to this file as PRs
   @Authors   https://github.com/MoonModules/MoonLight/commits/main
   @Doc       https://moonmodules.org/MoonLight/components/#fileedit
   @Copyright © 2025 Github MoonLight Commit Authors
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
-->

<script lang="ts">
	import Text from '$lib/components/Text.svelte';
	import Textarea from '$lib/components/Textarea.svelte';
	import File from '$lib/components/File.svelte';
	import type { FilesState } from '$lib/types/models';
	import { tick } from 'svelte';
    import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import Collapsible from '$lib/components/Collapsible.svelte';
	import { onMount } from 'svelte';

	export let path = "";
	export let showEditor = true;
	export let newItem = false;
	export let isFile = true;

	let folder:string = "";

	let formErrors = {
		name: false
	};

    let editableFile: FilesState = {
		name: '',
		path: path,
		isFile: isFile,
		size: 0,
		time: 0,
		contents: '',
		files: [],
		fs_total: 0,
		fs_used: 0,
	};

    async function postFilesState(data: any) { //export needed to call from other components
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
				return await response.json();
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
        return null; //no need to return anything!
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

    async function getFileContents() {
		console.log("getFileContents", path, path[0])
		editableFile.isFile = isFile;
		editableFile.path = path;
		if (newItem) {
			editableFile.name = '';
			folder = path + "/";
			editableFile.contents = '';
		} else {
			const parts = path.split('/');
			editableFile.name = parts.pop() || '';
			folder = parts.join('/') + "/";
			if (path[0] === '/') {
				const response = await fetch('/rest/file/' + editableFile.path, {
					method: 'GET',
					headers: {'Content-Type': 'text/plain'}
				});
				editableFile.contents = await response.text();
				// console.log("getFileContents", editableFile.contents)
			}
		}
    }

    //reactive response as soon as path changes, load new file contents
    $: if (editableFile.path != path) {
		getFileContents();
    }

	onMount(() => {
       	getFileContents();
	});


	function onSave() {
		console.log("onSave", editableFile.isFile)
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
			let response:any = {};
			if (newItem) {
				editableFile.path = folder + editableFile.name;
				// folderList.push(editableFile);
				//order by name ...

				//send newfile or folder to server

				// editableFile.path = "/" + breadCrumbs.join("/") + "/" + editableFile.name;
				response.news = [];
				response.news.push(editableFile);
				console.log("new item", response)
				//send the new itemstate to server
				
			} else {
				console.log("update item", editableFile)
				// folderList.splice(folderList.indexOf(editableFile), 1, editableFile);
				
				response.updates = [];
				response.updates.push(editableFile);
			}
			postFilesState(response);
			showEditor = false;
		}
	}

</script>

{#if path[0] === '/'}
	<Collapsible open={showEditor} class="shadow-lg" on:closed={console.log}>
		<span slot="title">{newItem ? 'Add ' + (isFile?"file":"folder") : 'Edit ' + editableFile.name}</span>
		<div class="divider my-0" />

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
		{#if isFile}
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
		<div class="mx-4 mb-4 flex flex-wrap justify-end gap-2">
			<button
				class="btn btn-primary"
				on:click={() => {
					console.log("Save");
					onSave();
				}}
			>
				Save</button
			>
		</div>
		<div class="divider mb-2 mt-0" />
	</Collapsible>
{/if}
