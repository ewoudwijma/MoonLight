<script lang="ts">
	import type { PageData } from './$types';
	import logo from '$lib/assets/logo.png';
	import { notifications } from '$lib/components/toasts/notifications';

	export let data: PageData;
</script>

<div class="hero bg-base-100 h-screen">
	<div class="card md:card-side bg-base-200 shadow-primary shadow-2xl">
		<figure class="bg-base-200"><img src={logo} alt="Logo" class="h-auto w-64" /></figure>
		<div class="card-body w-80">
			<h2 class="card-title text-center text-2xl">Welcome to MoonBase</h2>
			<p class="py-6 text-center">
				MoonBase is made by MoonModules, a group of LED enthusiasts who also made WLED MM 
				and contribute to WLED and can be found on 
				<a
					href="https://discord.gg/TC8NSUSCdV"
					class="link"
					target="_blank"
					rel="noopener noreferrer">Discord WLEDM MM 2D & Audio</a
				> and 
				<a
					href="https://moonmodules.org"
					class="link"
					target="_blank"
					rel="noopener noreferrer">MoonModules.org</a
				>.
			</p>
			<a
				class="btn btn-primary"
				href="/demo"
				on:click={() => notifications.success('You did it!', 1000)}>Start MoonBase</a
			>
		</div>
	</div>
</div>
