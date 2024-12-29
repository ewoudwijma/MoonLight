<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import FilesIcon from '~icons/tabler/files';
	import Save from '~icons/tabler/device-floppy';
	import Reload from '~icons/tabler/reload';

	onMount(() => {
	});

	onDestroy(() => {});

</script>

<SettingsCard collapsible={false}>
	<FilesIcon slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Files</span>
	<div class="w-full">
		<div class="flex flex-row flex-wrap justify-between gap-x-2">
			<div class="flex-grow" />
			<button class="btn btn-primary inline-flex items-center"
				><Save class="mr-2 h-5 w-5" /><span>Save</span></button
			>
			<button class="btn btn-primary inline-flex items-center"
				><Reload class="mr-2 h-5 w-5" /><span>Reload</span></button
			>
		</div>
	</div>
</SettingsCard>
