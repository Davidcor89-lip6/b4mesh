# Blockgraph live visualizer

Blockgraph nodes live rendering, in HTML/CSS

## Usage

Green's NGinx server might serve `live_visualizer.html` on a `/visualization` endpoint for instance.

## Configuration

The web-worker which polls datas to render, defined in `scripts/poll_worker_tasks.js`, use `configuration.js` as configuration file.  
Feel free to modify any variables in, in order to change the worker's behavior.

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

## Dependencies

This components relies on :

- D3
- WASM
- [d3-graphviz](https://github.com/magjac/d3-graphviz)

## Limitations

This component relies on two external components, in order to draw dot graphs *(see dependencies section)*.

However, such components do not currently support all dot-graph features, such as :

- Clusters outlines  
  ```dot
  subgraph cluster_MyClusterName
  {
      // content
  }
  ```
- ~~Orientations *(graphs are drawn in a top-bottom way only)*~~  
    **Fixed** with components update

## Tests

- Run a simple http server using Python :
  ```bash
   py ./tests/server.py
  ```

- Visit [http://127.0.0.1:8000/live_visualizer.html] using a web-browser *(but IE)*, or whatever url/port you specified

## Improvements / nice-to-have / todos

- [ ] Manage zoom/focus properly