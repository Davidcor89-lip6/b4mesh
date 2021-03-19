# Blockgraph live visualizer

Blockgraph nodes live rendering, in HTML/CSS + Javascript

## Usage

Green's NGinx server might serve `live_visualizer.html` on a `/visualization` endpoint for instance.

UI features are detailed [here](./UI_README.md)

## Deployement

See top-level README.md, `live visualisation` section

## Configuration

See top-level README.md, `live visualisation` section

Note about `live_visualizer_refresh_rate` parameter :

As this represents the data-polling frequency, the value must be chosoe carefully.  
In order to get a refresh rate lower then 1500 ms, you'd be willing to also lower both D3 animations delay and duration, in order to avoid any blinking.

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

This component relies on three external components, in order to draw dot graphs *(see dependencies section)*.

However, such components do not currently support all dot-graph features, such as :

- ~~Clusters outlines~~
- ~~Orientations *(graphs are drawn in a top-bottom way only)*~~  
    **Fixed** with components update

## Tests

- Run a simple http server using Python :
  ```bash
   py ./tests/server.py
  ```

- Visit [http://127.0.0.1:8000/live_visualizer.html] using a web-browser *(but IE)*, or whatever url/port you specified

## Improvements / nice-to-have / todos

- [X] Manage zoom
- [X] Manage centering/focus
- [X] Automate camera
- [X] Optimize memory usage *(2.4 Go is way to much)*
