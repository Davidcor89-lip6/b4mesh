// zoom, pan operations

var zoom = d3.zoom();
// .translateExtent([0, 0])
// .scaleExtent([.5, 20])
// .on("zoom", function(){
// })
var center_and_zoom_element = function (target, scale_modifier = 4) {
  // set `scale_modifier` to `null` to reset zoom
  // rendering is then defered to the caller

  var bbox = target.getBBox();
  var element = d3.select(target);

  var transform = d3.zoomTransform(target);
  const element_bbox = target.getBBox(); // test element.node().getBBox();

  if (element_bbox === undefined) return; // d3/rendering not initialized yet

  const svg_bbox = d3.select("div").select("svg").select("g").node().getBBox();
  const scale = scale_modifier === null ? 1 : transform.k + scale_modifier;

  const translate_to = {
    x: (-element_bbox.x - element_bbox.width / 2) * scale + svg_bbox.width / 2,
    y:
      (-element_bbox.y - element_bbox.height / 2) * scale + svg_bbox.height / 2,
  };

  d3.select("svg").call(
    zoom.transform,
    d3.zoomIdentity.translate(translate_to.x, translate_to.y).scale(scale)
  );
  // .on("end", function(){
  //     rendering.render();
  // })
};
var center_element = function (target) {
  center_and_zoom_element(target, 0);
};
