<script lang="ts">
    export let onClick: () => void;
    export let label: string;
</script>

<button class="btn btn-primary inline-flex items-center" on:click={onClick}
    ><slot></slot><span>{label}</span></button
>