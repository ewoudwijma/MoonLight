<script lang="ts">
	import { onMount, onDestroy } from 'svelte';
	import { user } from '$lib/stores/user';
	import { page } from '$app/stores';
	import { notifications } from '$lib/components/toasts/notifications';
	import SettingsCard from '$lib/components/SettingsCard.svelte';
	import LightIcon from '~icons/tabler/bulb';
	import { socket } from '$lib/stores/socket';
	import type { FixtureState } from '$lib/types/models';
	import Slider from '$lib/components/Slider.svelte';
	import Checkbox from '$lib/components/Checkbox.svelte';
	import Number from '$lib/components/Number.svelte';
	import Spinner from '$lib/components/Spinner.svelte';

	let fixtureState: FixtureState;

	//fixtureState is now via socket and not rest api ...
	let dataLoaded = false;
	
	async function getState() {
		try {
			const response = await fetch('/rest/fixtureState', {
				method: 'GET',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				}
			});
			fixtureState = await response.json();
			dataLoaded = true;
		} catch (error) {
			console.error('Error:', error);
		}
		return;
	}

	onMount(() => {
		socket.on<FixtureState>('fixture', (data) => {
			fixtureState = data;
			dataLoaded = true;
		});
		// getState(); //done in settingscard
	});

	onDestroy(() => socket.off('fixture'));

	function sendSocket() {
		if (dataLoaded) 
			socket.sendEvent('fixture', fixtureState)
	}

	async function postFixtureState() {
		try {
			const response = await fetch('/rest/fixtureState', {
				method: 'POST',
				headers: {
					Authorization: $page.data.features.security ? 'Bearer ' + $user.bearer_token : 'Basic',
					'Content-Type': 'application/json'
				},
				body: JSON.stringify({ brightness: fixtureState.brightness, lightsOn: fixtureState.lightsOn })
			});
			if (response.status == 200) {
				notifications.success('fixtureState updated.', 3000);
				fixtureState = await response.json();
			} else {
				notifications.error('User not authorized.', 3000);
			}
		} catch (error) {
			console.error('Error:', error);
		}
	}
</script>

<SettingsCard collapsible={false}>
	<LightIcon slot="icon" class="lex-shrink-0 mr-2 h-6 w-6 self-end" />
	<span slot="title">Fixture</span>
	{#await getState()}
		<Spinner />
	{:then nothing}
	<div class="w-full">
		<Checkbox 
			label="On" 
			bind:value={fixtureState.lightsOn}
			onChange={sendSocket}
		></Checkbox>
		<Slider 
			label="Brightness" 
			bind:value={fixtureState.brightness}
			min={0} 
			max={255} 
			step={1}
			onChange={sendSocket}
		></Slider>
		<Number 
			label="Width" 
			bind:value={fixtureState.width} 
			onChange={sendSocket}
		></Number>
		<Number 
			label="Height" 
			bind:value={fixtureState.height} 
			onChange={sendSocket}
		></Number>
		<Number 
			label="Depth" 
			bind:value={fixtureState.depth} 
			onChange={sendSocket}
		></Number>
		<Checkbox 
			label="Driver on" 
			bind:value={fixtureState.driverOn}
			onChange={sendSocket}
		></Checkbox>
		<Checkbox 
			label="Monitor on" 
			bind:value={fixtureState.monitorOn}
			onChange={sendSocket}
		></Checkbox>
		<Number 
			label="Pin" 
			bind:value={fixtureState.pin} 
			onChange={sendSocket}
		></Number>
	
	</div>
	{/await}
</SettingsCard>
