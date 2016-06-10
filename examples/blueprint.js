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
    
    var width = width(), // was: 500,
        height = height() // was: 500;

    var diameter = Math.min(width - 200, height) // was: 300;
    var duration = 750 // was: 2000;

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

    var radialRootTransform = "translate(" + (width / 2) + "," + (height / 2) + ")scale(1,0.666)"

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

        old_links.remove()

        function startPath(d) {
            var from = d.source.y + "," + d.source.x,
                to = d.target.y + "," + d.target.x
                //return "M" + to + "L" + from
            return "M" + from + "L" + to
        }

        new_links = new_links.append("path")
            .attr("class", "link")
            .style("stroke", linkStroke)
            .attr("d", startPath)
            .style("opacity", 0.0)

        all_links.transition()
            .duration(duration)
            .style("stroke", linkStroke)
            .attr("d", linkPath) //get the new cluster path
            .style("opacity", 0.5)

        var old_nodes = all_nodes.exit()
        var new_nodes = all_nodes.enter()

        //console.log("NODES", new_nodes[0].length, all_nodes[0].length, old_nodes[0].length)

        old_nodes.remove()

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
            .attr("class", "node")
            .attr("transform", nodeTransform)
            .style("opacity", 0.2)

        new_nodes.append("circle")
            .attr("r", 0) // nodeRadius)
            .style("stroke", nodeStroke)
            .style("fill", nodeFill)
            .on("click", function (d) {
                var toggle = d3.event.metaKey || d3.event.altKey || d3.event.ctrlKey
                var modify = d3.event.shiftKey
                d.click(toggle, modify)
                d3.event.stopPropagation()
            })

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

        new_nodes.transition()
            .duration(duration)

        all_nodes.transition()
            .duration(duration)
            .attr("transform", nodeTransform)
            .style("opacity", 1.0)

        all_nodes.select("circle")
            .transition()
            .duration(duration)
            .attr("r", nodeRadius)
            .style("stroke", nodeStroke)
            .style("fill", nodeFill)
    }

    var root; // store data in a variable accessible by all functions

    var diagonal = d3.svg.diagonal()
        .projection(function (d) {
            return [d.y, d.x];
        });

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

    var radialDiagonal = d3.svg.diagonal.radial()
        .projection(function (d) {
            return [d.y, d.x / 180 * Math.PI];
        })

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

    // There can only be one... kill old svg and make a new
    var svg = d3.select("body").select("svg").remove();
    svg = d3.select("body").append("svg")
        .attr("width", width)
        .attr("height", height)
        .on("click", toggleDisplay)
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
        function iframe(caption, type) {
            // Update the display with the name
            frame.setAttribute("caption", caption || title || "No Name")
            
            //console.log("IFRAME", frame.dataset.caption, caption, title, this, node)
            
            var iframe = frame.lastElementChild instanceof HTMLIFrameElement && frame.lastElementChild
            // Check for content
            if (node) {
                if (!iframe) {
                    iframe = document.createElement("iframe")
                    //iframe.setAttribute("tabindex", "0")
                }
                if (!iframe) throw "TEST FAILED"
                iframe.src = "data:"+(type||"text/plain;charset=utf8")+","+encodeURI(node.code())
                // Last child is always the iframe
                if (iframe !== frame.lastElementChild) {
                    frame.appendChild(iframe)
                }
                // Make iframe visible
                frame.classList.add("display")
            }
            // Remove old iframe (this get's rid of the source as well)
            else if (iframe) {
                console.log("HIDE")
                frame.removeChild(iframe)
                delete frame.style.height
            }
        }

        var code = d3.select(frame)
        /***
        var same = (node && node.display === frame)
        if (same) {
            // TODO: what about repeated clicks?
            // delete node.display
            // Make iframe invisible
            // frame.classList.remove("display")
            // toggleDisplay()
        } 
        else 
        ***/
        if (node) {
            // Show content in iframe
            var type = node.type
            iframe(title || node.name, type)
            node.display = frame
            frame.classList.add("display")
        }
        else {
            iframe(title)
        }
    }

    function toggleDisplay() {
        var DISPLAY = "display"
        if (frame.classList.contains(DISPLAY)) {
            frame.classList.remove(DISPLAY)
            delete frame.style.height // remove explicit set height
        }
        else {
            frame.classList.add(DISPLAY)
        }
    }    
    
    /* DRAG'n'DROP */
/***
    function dragDisplay(event) {
        console.log("DRAG", event.target)
    }

    function dragStartDisplay(event) {
        console.log("DRAG start", this, event.target)
        event.target.style.opacity = 0.31415926
        event.dataTransfer.setData('text/plain', this.title)
        event.effectAllowed = 'move';
    }
    
    function dragEnterDisplay(event) {
        console.log("DRAG enter", this, event.target)
        event.preventDefault();
    }
    
    function dragOverDisplay(event) {
        console.log("DRAG over", this, event.target)
        event.dataTransfer.dropEffect = 'move'  // See the section on the DataTransfer object.
        event.preventDefault() // Necessary. Allows us to drop.
    }

    function dragEndDisplay(event) {
        console.log("DRAG end", event)
        event.target.style.opacity = 1
    }
    
    function dropDisplay(event) {
        console.log("DROP", event)
    }

    frame.draggable = true
        
    // workspace.addEventListener('drag', dragDisplay, false)
    
    frame.addEventListener('dragstart', dragStartDisplay, false)
    
    workspace.addEventListener('dragenter', dragEnterDisplay, false)
    workspace.addEventListener('dragover', dragOverDisplay, false)
    workspace.addEventListener('drop', dropDisplay, false)
    
    frame.addEventListener('dragend', dragEndDisplay, false)
    //workspace.addEventListener('dragstop', dragEndDisplay, false)
    
***/

    // Interface to XENO:
    return {
        update: function () {
            if (transitionToLayout) transitionToLayout()
        },
        display: display,
        toggleDisplay: toggleDisplay,
    }

} // XENO