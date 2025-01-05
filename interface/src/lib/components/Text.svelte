<script lang="ts">
    export let label: string;
    export let value: string;
    export let onChange = () => {};
</script>

<label class="label cursor-pointer" for={label}>
    <span class="mr-4">{label}</span>
</label>
<input 
    type="text"
    class="input"
    id={label}
    bind:value={value}
    on:change={onChange}
/>