# Blockgraph live visualizer

Blockgraph nodes live rendering, in HTML/CSS

## Usage

Green's NGinx server might serve `live_visualizer.html` on a `/visualization` endpoint for instance.

## Configuration

In `scripts/poll_worker_tasks.js`, see configuration object :

```js
const configuration = {
    blockgraph_as_dot_API_accesspoint : 'http://127.0.0.1:8000/tests/datas/blockgraph.dot.json',
    refresh_rate_ms : 3000
};
```

Thus, assign to `blockgraph_as_dot_API_accesspoint` the endpoint to poll a list of JSON values from,  
and `refresh_rate_ms` to the wished data-polling frequency.

## Input datas

Input datas are polled using a `Worker`, configured as mentionned in the previous section.  

Requesting `blockgraph_as_dot_API_accesspoint` endpoint, the visualizer expect a list of `\n`-separated json entries, as describe hereunder :

```json
{
    "node": {
        "groupId":/* integer */,
        "hash": /* integer */,
        "parent":[/*integer, ...*/]
    }
}
```