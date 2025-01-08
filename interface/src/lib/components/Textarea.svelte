<script lang="ts">
    export let label: string;
    export let value: string;
    export let onChange = () => {};
</script>

<label class="label cursor-pointer" for={label}>
    <span class="mr-4">{label}</span>
</label>
<textarea rows="10"
    class="input"
    id={label}
    on:change={onChange}
>{value}</textarea>