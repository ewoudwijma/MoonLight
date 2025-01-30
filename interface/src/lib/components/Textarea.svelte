<script lang="ts">
    export let label: string;
    export let value: string;
    export let onChange = (event:any) => {console.log(event)};
</script>

<label class="label cursor-pointer" for={label}>
    <span class="mr-4">{label}</span>
</label>
<textarea rows="10" cols="65"
    class="textarea textarea-bordered"
    id={label}
    on:change={onChange}
>{value}</textarea>