function XENO(root, frame) {

    // Hack to determine window dimensions
    function width() {
        return window.innerWidth 
            || document.documentElement.clientWidth 
            || document.body.clientWidth 
            || 1024;
    }

    function height() {
        return window.innerHeight 
            || document.documentElement.clientHeight 
            || document.body.clientHeight 
            || 800;
    }
    
    // Substract padding 10px from dimensions
    var width = width() - 20, // was: 500,
        height = height() - 20 // was: 500; 
        

    var diameter = Math.min(width - 200, height) // was: 300;
    var duration = 750 // was: 2000;

    if (width > height) height = width, diameter = width - 200

    // Use the selection as default for layout
    var layout_options = d3.select("form").node().layout
    var default_layout = layout_options.value
    var transitionToLayout = eval("transitionTo" + default_layout)
    // Change layout
    d3.select(layout_options).on("change", function change() {
        transitionToLayout = eval("transitionTo" + this.value)
        transitionToLayout()
    })

    // Nodes and link get ID for object persistence in transitions
    
    var globalNodeID = 0
    function nodeID(d) {
        return d.id || (d.id = ++globalNodeID)
    }
    var globalLinkID = 0
    function linkID(d) {
        return d.id || (d.id = d.source.id + ">" + d.target.id)
    }

    // Transition functions

    // Radial systems

    var radialRootTransform = "translate(" + (width / 2) + "," + (100 + height * 0.333) + ")scale(1,0.666)"

    function radialNodeTransform(d) {
        if (d.parent)
            return "rotate(" + (d.x - 90) + ")translate(" + d.y + ")"
        else
            return ""
    }

    function transitionToRadialTree() {
        transitionTo(radialTree, radialRootTransform, radialNodeTransform, "#a5731d", radialDiagonal, "#fce0b1")
    }

    function transitionToRadialCluster() {
        transitionTo(radialCluster, radialRootTransform, radialNodeTransform, "#aabbaa", radialDiagonal, "#d2e0d2")
    }

    // Cartesian systems

    var cartesianRootTransform = "translate(100,0)"

    function cartesianNodeTransform(d) {
        return "translate(" + d.y + "," + d.x + ")"
    }

    function transitionToTree() {
        transitionTo(tree, cartesianRootTransform, cartesianNodeTransform, "#377eb8", diagonal, "#dce7ff")
    }

    function transitionToCluster() {
        transitionTo(cluster, cartesianRootTransform, cartesianNodeTransform, "#ce8fac", diagonal, "#f8dce9")
    }


    // General transition function

    function transitionTo(layout, rootTransform, nodeTransform, nodeStroke, linkPath, linkStroke) {
        // Recalculate layout
        var nodes = layout.nodes(root),
            links = layout.links(nodes);

        svg.transition().duration(duration)
            .attr("transform", rootTransform);

        var all_nodes = svg_nodes.selectAll(".node").data(nodes, nodeID)

        var all_links = svg_links.selectAll(".link").data(links, linkID)

        var old_links = all_links.exit()
        var new_links = all_links.enter()

        //console.log("LINKS", new_links[0].length, all_links[0].length, old_links[0].length)

        function startPath(d) {
            var from = d.source.y + "," + d.source.x,
                to = d.target.y + "," + d.target.x
            return "M" + to + "L" + from
            // return "M" + from + "L" + to
        }

        function startParent(d) {
            var from = d.source.y + "," + d.source.x,
                to = d.target.y + "," + d.target.x
            if (d.target.parent) 
                to = d.target.parent.y + "," + d.target.parent.x
            return "M" + to + "L" + from
            // return "M" + from + "L" + to
        }

        old_links
            .attr("class", "link fadeout")
            .transition()
            .duration(duration)
            // .style("opacity", 0.0)
            .remove()

        new_links = new_links.append("path")
            .attr("class", "link fadein")
            .style("stroke", linkStroke)
            .attr("d", startParent) // linkPath) // 
            // .style("opacity", 0.0)

        all_links.transition()
            .duration(duration)
            .style("stroke", linkStroke)
            .attr("d", linkPath) //get the new cluster path
            // .style("opacity", 0.5)

        var old_nodes = all_nodes.exit()
        var new_nodes = all_nodes.enter()

        //console.log("NODES", new_nodes[0].length, all_nodes[0].length, old_nodes[0].length)

        old_nodes
            .attr("class", "node fadeout")
            .transition()
            .duration(duration)
            // .style("opacity", 0.0)
            .remove()

        function rgbToHex(r, g, b) {
            return "#" + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
        }
        
        function hexToRgb(hex) {
            // Expand shorthand form (e.g. "03F") to full form (e.g. "0033FF")
            var shorthandRegex = /^#?([a-f\d])([a-f\d])([a-f\d])$/i;
            hex = hex.replace(shorthandRegex, function(m, r, g, b) {
                return r + r + g + g + b + b;
            });
            // Pick out hex digit pairs
            var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
            if (result) return {
                r: parseInt(result[1], 16),
                g: parseInt(result[2], 16),
                b: parseInt(result[3], 16),
                toRGBa: function (a) {
                    var c = [this.r, this.g, this.b, a];
                    return 'rgba('+c.join(',')+')'
                }
            }
            // Might be an rgb
            var result = /rgb\((\d{1,3}), (\d{1,3}), (\d{1,3})\)/i.exec(hex)
            if (result) return {
                r: parseInt(result[1], 10),
                g: parseInt(result[2], 10),
                b: parseInt(result[3], 10),
                toRGBa: function (a) {
                    var c = [this.r, this.g, this.b, a];
                    return 'rgba('+c.join(', ')+')'
                }
            }
            return null;
        }
        
        function nodeFill(d) {
            return d.children ? null : d.size === 1 ? null : hexToRgb(this.style.stroke).toRGBa(0.5)
        }

        function nodeRadius(d) {
            return d.children ? 4.5 : 4.5 * Math.sqrt(Math.sqrt(d.size))
        }

        new_nodes = new_nodes.append("g")
            .attr("class", "node fadein")
            .attr("tabindex", "0") // make focusable
            .attr("transform", nodeTransform)
            // .style("opacity", 0.2)

        new_nodes.append("circle")
            .attr("r", 0) // nodeRadius)
            .style("stroke", nodeStroke)
            .style("fill", nodeFill)

        new_nodes.append("text")
            .attr("dx", function (d) {
                return d.size > 1 ? -nodeRadius(d) - 4.5 : +nodeRadius(d) + 4.5
            })
            // .attr("dy", 0)
            .style("text-anchor", function (d) {
                return d.size > 1 ? "end" : "start"
            })
            .text(function (d) {
                return d.name
            })

        //new_nodes.transition()
          //  .duration(duration)

        all_nodes.transition()
            .duration(duration)
            .attr("transform", nodeTransform)
            //.style("opacity", 1.0)

        all_nodes.select("circle")
            .transition()
            .duration(duration)
            .attr("r", nodeRadius)
            .style("stroke", nodeStroke)
            .style("fill", nodeFill)
    }

    var root; // store data in a variable accessible by all functions

    var nodeWidth = 100,
        nodeHeight = 18

    var tree = d3.layout.tree()
        //    .nodeSize([nodeHeight, nodeWidth])
        //    .separation(function(a, b) {
        //        return 18*(a.parent == b.parent ? 1 : 2)
        //    })
        .size([height, width - 250])

    var cluster = d3.layout.cluster()
        .size([height, width - 250])

    var diagonal = d3.svg.diagonal()
        .projection(function (d) {
            return [d.y, d.x];
        });

    var radialTree = d3.layout.tree()
        .size([360, diameter / 2])
        .separation(function (a, b) {
            return (a.parent == b.parent ? 1 : 2) / a.depth;
        })

    var radialCluster = d3.layout.cluster()
        .size([360, diameter / 2])
        .separation(function (a, b) {
            return (a.parent == b.parent ? 1 : 2) / a.depth;
        })

    var radialDiagonal = d3.svg.diagonal.radial()
        .projection(function (d) {
            return [d.y, d.x / 180 * Math.PI];
        })

    // There can only be one... kill old svg and make a new
    
    d3.select("#workspace .svg-container svg").remove()
    var svg = d3.select("#workspace .svg-container")
        .append("svg")
            // responsive SVG needs these 2 attributes and no width and height attr
            .attr("preserveAspectRatio", "xMinYMin meet")
            .attr("viewBox", "0 0 "+width+" "+height)
            // class to make it responsive
            .classed("svg-content-responsive", true)
            .attr("width", width)
            .attr("height", height)
            .append("g")

    var svg_links = svg.append("g").attr("class", "links")
    var svg_nodes = svg.append("g").attr("class", "nodes")
    
    // All setup, kick of the SVG
    if (transitionToLayout) transitionToLayout()
    
    // Display section
    var frame = frame, // remember given frame
        workspace = frame.parentNode
    
    function display(node, title) {
/**
        console.log("DISPLAY", title, this, node)
        function render(lang) {
            var s = node.context.code()
            s = s.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
            s = "<code class='lang-" + lang + "'>" + s + "</code>";
            return s;
        }
**/       
        function iframe(caption, type, url) {
            // Update the display with the name
            frame.setAttribute("title", caption || title || "No Name")
            
            //console.log("IFRAME", frame.dataset.caption, caption, title, this, node)
            
            var iframe = frame.lastElementChild instanceof HTMLIFrameElement && frame.lastElementChild
            // Check for content
            if (node) {
                if (!iframe) {
                    iframe = document.createElement("iframe")
                    iframe.setAttribute("tabindex", "0")
                    //iframe.setAttribute("onload", "this.contentWindow.focus()") 
                }
                if (!iframe) throw "Could not create an iframe"
                var data = encodeURIComponent(node.code())
                //var preamble = '<!DOCTYPE html>'
                //function wrapDiv(s) { return "<div>" + s + "</div>"}
                iframe.src = "data:" + (type||"text/plain;charset=utf8") + "," + data // preamble + wrapDiv(data)
                // Last child is always the iframe
                if (iframe !== frame.lastElementChild) {
                    frame.appendChild(iframe)
                }
                // Make iframe visible
                frame.classList.add("display")
                // Install a monitor to check when the iframe gets focus
                var monitor = setInterval(function poll(){
                  var elem = document.activeElement;
                  if (elem === iframe){
                      clearInterval(monitor);
                      //frame.focus();
                      iframe.focus();
                      monitor = setInterval(poll, 100);
                  }
                }, 100);
            }
            // Remove old iframe (this get's rid of the source as well)
            else if (iframe) {
                //console.log("HIDE")
                frame.removeChild(iframe)
                delete frame.style.height
            }
        }

        var code = d3.select(frame)
/*        
        var same = (node && node.display === frame)
        if (same) {
            // TODO: what about repeated clicks?
            delete node.display
            // Make iframe invisible
            frame.classList.remove("display")
            // toggleDisplay()
        }
        else 
*/
        if (node) {
            // Show content in iframe
            var type = node.type || getMimeType(node)
            iframe(title || node.name, type)
            node.display = frame
            frame.classList.add("display")
        }
        else {
            iframe(title)
        }
    }
    
    function getMimeType(node) {
        if (node.origin.tagName == 'html') return 'text/html;charset=utf-8';
        if (node.origin.tagName == 'svg') return 'image/svg+xml;charset=utf-8';
        return undefined;
    }

    function toggleDisplay() {
        //console.log("XENO.toggleDisplay")
        var DISPLAY = "display"
        if (frame.classList.contains(DISPLAY)) {
            frame.classList.remove(DISPLAY)
            delete frame.style.height // remove explicit set height
        }
        else {
            frame.classList.add(DISPLAY)
        }
    }    
    
    // Interface to XENO:
    return {
        update: function () {
            if (transitionToLayout) transitionToLayout()
        },
        display: display,
        toggleDisplay: toggleDisplay,
    }

} // XENO

// https://raw.githubusercontent.com/anonyco/BestBase64EncoderDecoder/master/atobAndBtoaTogether.min.js
!function(e){"use strict";function h(b){var a=b.charCodeAt(0);if(55296<=a&&56319>=a)if(b=b.charCodeAt(1),b===b&&56320<=b&&57343>=b){if(a=1024*(a-55296)+b-56320+65536,65535<a)return d(240|a>>>18,128|a>>>12&63,128|a>>>6&63,128|a&63)}else return d(239,191,189);return 127>=a?b:2047>=a?d(192|a>>>6,128|a&63):d(224|a>>>12,128|a>>>6&63,128|a&63)}function k(b){var a=b.charCodeAt(0)<<24,f=l(~a),c=0,e=b.length,g="";if(5>f&&e>=f){a=a<<f>>>24+f;for(c=1;c<f;++c)a=a<<6|b.charCodeAt(c)&63;65535>=a?g+=d(a):1114111>=a?(a-=65536,g+=d((a>>10)+55296,(a&1023)+56320)):c=0}for(;c<e;++c)g+="\ufffd";return g}var m=Math.log,n=Math.LN2,l=Math.clz32||function(b){return 31-m(b>>>0)/n|0},d=String.fromCharCode,p=atob,q=btoa;e.btoaUTF8=function(b,a){return q((a?"\u00ef\u00bb\u00bf":"")+b.replace(/[\x80-\uD7ff\uDC00-\uFFFF]|[\uD800-\uDBFF][\uDC00-\uDFFF]?/g,h))};e.atobUTF8=function(b,a){a||"\u00ef\u00bb\u00bf"!==b.substring(0,3)||(b=b.substring(3));return p(b).replace(/[\xc0-\xff][\x80-\xbf]*/g,k)}}(""+void 0==typeof global?""+void 0==typeof self?this:self:global)//anonyco
