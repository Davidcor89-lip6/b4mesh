var json_to_dot_cluster = function (datas, options = {}) {
  // parsing
  const re = /\{\"node\"\:\{\"groupId\"\:(\d+)\,\"hash\"\:(\d+)\,\"parent\"\:\[(.*)\]\}\}/gm;
  var matches = re[Symbol.matchAll](datas);
  var parsed_datas = Array.from(matches);

  var groups_map = {};
  parsed_datas.forEach(function (item) {
    var group_id = item[1];
    var hash_id = item[2];
    var parents = item[3];

    if (groups_map[group_id] == undefined) groups_map[group_id] = new Array();
    groups_map[group_id].push([hash_id, parents.split(",")]);
  });

  // generation
  var output = "digraph blockGraph_transactions {\n";
  output += "rankdir=" + options + ";\n";
  output += "node [shape=box fontname=Arial]";

  // to avoid wrong clustering, first print clusters
  // with their content (nodes)
  output += "// clusters and nodes ...\n";
  for (var group_id in groups_map) {
    output += "subgraph cluster_" + group_id + " {\n";
    output += "label=" + group_id + ";";
    // output += "mode=\"hier\"\n";
    output += "rankdir=" + options + ";\n";
    output += 'node[group=""];\n';
    var count = 0;
    groups_map[group_id].forEach(function (item) {
      var hash_id = item[0];
      var parents_array = item[1];

      for (var index in parents_array) {
        output += hash_id + (++count % 5 == 0 ? ";\n" : ";");
      }
    });
    output += "\n}";
  }
  // then nodes relationships
  output += "// relationships ...\n";
  for (var group_id in groups_map) {
    groups_map[group_id].forEach(function (item) {
      var hash_id = item[0];
      var parents_array = item[1];

      for (var index in parents_array) {
        output += parents_array[index] + " -> " + hash_id + ";\n";
      }
    });
  }
  output += "}";
  return output;
};

// const datas = `{"node":{"groupId":217,"hash":7144646,"parent":[9288542]}}
// {"node":{"groupId":26,"hash":7377976,"parent":[2681597,8515655]}}`;
// console.log(json_to_dot(datas));
