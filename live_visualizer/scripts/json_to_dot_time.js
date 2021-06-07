//dic [id]-> counter
//counter for each id (to know wich id have already appeared and how many time)

var group_id_dico = {};
var tab_id = [];
var length_id = 0;
//dic [hash]->id
var dico_parento_to_id = {};
var nb_cluster = function () {
  return length_id;
};
var json_to_dot_time = function (datas, options = {}) {
  // parsing
  const re = /\{\"node\"\:\{\"groupId\"\:(\d+)\,\"hash\"\:(\d+)\,\"parent\"\:\[(.*)\]\}\}/gm;
  var matches = re[Symbol.matchAll](datas);
  var parsed_datas = Array.from(matches);
  var groups_map = {};

  parsed_datas.forEach(function (item) {
    if (!tab_id.includes(item[1])) {
      tab_id.push(item[1]);
    }
    dico_parento_to_id[item[2]] = item[1];
    //part to distinguish  cluster
    var splitparent = item[3].split(",");
    if (splitparent.length > 1) {
      if (group_id_dico[item[1]] == undefined) {
        group_id_dico[item[1]] = 0;
      } else {
        group_id_dico[item[1]] += 1;
      }
      //treatment for the potential of having a split with an id already seen (its parents id)
      for (const parent_hash of splitparent) {
        if (group_id_dico[dico_parento_to_id[parent_hash]] == undefined) {
          group_id_dico[dico_parento_to_id[parent_hash]] = 0;
        } else {
          group_id_dico[dico_parento_to_id[parent_hash]] += 1;
        }
      }
    }
    // separation of cluster with same id
    if (group_id_dico[item[1]] == undefined) {
      var group_id = item[1];
    } else {
      var group_id = item[1] + "_" + group_id_dico[item[1]];
    }

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
    if (group_id.split("_").length > 1) {
      output += 'label="' + group_id.split("_")[0] + '";';
    } else {
      output += 'label="' + group_id + '";';
    }

    // output += "mode=\"hier\"\n";
    output += "rankdir=" + options + ";\n";
    // output += "node[group=\"\"];\n"
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
  length_id = tab_id.length;

  //reset
  tab_id = [];
  group_id_dico = {};
  dico_parento_to_id = {};
  return output;
};

// const datas = `{"node":{"groupId":217,"hash":7144646,"parent":[9288542]}}
// {"node":{"groupId":26,"hash":7377976,"parent":[2681597,8515655]}}`;
// console.log(json_to_dot(datas));
