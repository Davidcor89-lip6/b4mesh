importScripts('../configuration.js')
importScripts('./string_hash.js')

var myRequest = new Request(configuration.blockgraph_as_dot_API_accesspoint);
setInterval(() => {
    fetch(myRequest, {cache: "reload"}) // always force cache
        .then(function(reponse) {
            return reponse.text();
        })
        .then(data => {
            // avoid unecessary messages (same datas)
            var data_hashCode = data.hashCode();
            if (this.cache === data_hashCode)
            {
                console.log('[poll_worker::setInterval] polled same datas, canceling unecessary datas update ...')
                return;
            }
            console.log('[poll_worker::setInterval] polled fresh datas, processing message ...')
            this.cache = data_hashCode;
            self.postMessage(data);
        })
}, configuration.refresh_rate_ms);
