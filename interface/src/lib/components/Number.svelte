<script lang="ts">
    export let label: string;
    export let value: number;
    export let onChange = () => {};
</script>

<label class="label cursor-pointer" for={label}>
    <span class="mr-4">{label}</span>
</label>
<input 
    type="number"
    class="input"
    id={label}
    min=-65000
    max=65000
    bind:value={value}
    on:change={onChange}
/>