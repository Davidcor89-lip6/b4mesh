# Blockgraph live visualizer

Blockgraph nodes live rendering, in HTML/CSS + Javascript

## Usage

Green's NGinx server might serve `live_visualizer.html` on a `/visualization` endpoint for instance.

UI features are detailed [here](./UI_README.md)

## Deployement

A CMake script is provide in order to handle - *most of* - the deployement process.  
It might be use independently, or as part of a top-level project.

Default behavior will append `live_visualiser` to [CMAKE_INSTALL_PREFIX](https://cmake.org/cmake/help/latest/variable/CMAKE_INSTALL_PREFIX.html#variable:CMAKE_INSTALL_PREFIX) variable, in order to avoid path collisions with an hypothetical top-level project.  
Such behavior can be override by setting `live_visualizer_install_dir` variable.

```bash
cd /path/to/b4mesh/live_visualizer;
mkdir cmake_build && cd cmake_build;
```

- Default :

    ```bash
    cmake ..;
    ```
- Using install prefix :

    ```bash
    cmake -DCMAKE_INSTALL_PREFIX=/path/to/installation/dir ..;
    ```
- Overriding install destination (target only)

    ```bash
    cmake -Dlive_visualizer_install_dir=/path/to/installation/dir ..;
    ```

Then simply install :

```bash
cmake --install .;
```

NB : This will install everything **but** the configuration *(see `b4mesh/deploy/`)*.

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

In order to get a `refresh_rate_ms` lower then 1500 ms, you'd be willing to also lower both D3 animations delay and duration, in order to avoid any blinking.

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
