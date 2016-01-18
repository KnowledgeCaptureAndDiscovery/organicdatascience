//
//  arbor.js - version 0.91
//  a graph vizualization toolkit
//
//  Copyright (c) 2016 Samizdat Drafting Co.
//  Physics code derived from springy.js, copyright (c) 2010 Dennis Hotson
// 
//  Permission is hereby granted, free of charge, to any person
//  obtaining a copy of this software and associated documentation
//  files (the "Software"), to deal in the Software without
//  restriction, including without limitation the rights to use,
//  copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the
//  Software is furnished to do so, subject to the following
//  conditions:
// 
//  The above copyright notice and this permission notice shall be
//  included in all copies or substantial portions of the Software.
// 
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//  OTHER DEALINGS IN THE SOFTWARE.
//
(function($) {

    /*        etc.js */
    var trace = function(msg) {
        if (typeof(window) == "undefined" || !window.console) {
            return
        }
        var len = arguments.length;
        var args = [];
        for (var i = 0; i < len; i++) {
            args.push("arguments[" + i + "]")
        }
        eval("console.log(" + args.join(",") + ")")
    };
    var dirname = function(path) {
        var pth = path.replace(/^\/?(.*?)\/?$/, "$1").split("/");
        pth.pop();
        return "/" + pth.join("/")
    };
    var basename = function(path) {
        var pth = path.replace(/^\/?(.*?)\/?$/, "$1").split("/");
        var base = pth.pop();
        if (base == "") {
            return null
        } else {
            return base
        }
    };
    var _ordinalize_re = /(\d)(?=(\d\d\d)+(?!\d))/g;
    var ordinalize = function(num) {
        var norm = "" + num;
        if (num < 11000) {
            norm = ("" + num).replace(_ordinalize_re, "$1,")
        } else {
            if (num < 1000000) {
                norm = Math.floor(num / 1000) + "k"
            } else {
                if (num < 1000000000) {
                    norm = ("" + Math.floor(num / 1000)).replace(_ordinalize_re, "$1,") + "m"
                }
            }
        }
        return norm
    };
    var nano = function(template, data) {
        return template.replace(/\{([\w\-\.]*)}/g, function(str, key) {
            var keys = key.split("."),
                value = data[keys.shift()];
            $.each(keys, function() {
                if (value.hasOwnProperty(this)) {
                    value = value[this]
                } else {
                    value = str
                }
            });
            return value
        })
    };
    var objcopy = function(old) {
        if (old === undefined) {
            return undefined
        }
        if (old === null) {
            return null
        }
        if (old.parentNode) {
            return old
        }
        switch (typeof old) {
            case "string":
                return old.substring(0);
                break;
            case "number":
                return old + 0;
                break;
            case "boolean":
                return old === true;
                break
        }
        var newObj = ($.isArray(old)) ? [] : {};
        $.each(old, function(ik, v) {
            newObj[ik] = objcopy(v)
        });
        return newObj
    };
    var objmerge = function(dst, src) {
        dst = dst || {};
        src = src || {};
        var merge = objcopy(dst);
        for (var k in src) {
            merge[k] = src[k]
        }
        return merge
    };
    var objcmp = function(a, b, strict_ordering) {
        if (!a || !b) {
            return a === b
        }
        if (typeof a != typeof b) {
            return false
        }
        if (typeof a != "object") {
            return a === b
        } else {
            if ($.isArray(a)) {
                if (!($.isArray(b))) {
                    return false
                }
                if (a.length != b.length) {
                    return false
                }
            } else {
                var a_keys = [];
                for (var k in a) {
                    if (a.hasOwnProperty(k)) {
                        a_keys.push(k)
                    }
                }
                var b_keys = [];
                for (var k in b) {
                    if (b.hasOwnProperty(k)) {
                        b_keys.push(k)
                    }
                }
                if (!strict_ordering) {
                    a_keys.sort();
                    b_keys.sort()
                }
                if (a_keys.join(",") !== b_keys.join(",")) {
                    return false
                }
            }
            var same = true;
            $.each(a, function(ik) {
                var diff = objcmp(a[ik], b[ik]);
                same = same && diff;
                if (!same) {
                    return false
                }
            });
            return same
        }
    };
    var objkeys = function(obj) {
        var keys = [];
        $.each(obj, function(k, v) {
            if (obj.hasOwnProperty(k)) {
                keys.push(k)
            }
        });
        return keys
    };
    var objcontains = function(obj) {
        if (!obj || typeof obj != "object") {
            return false
        }
        for (var i = 1, j = arguments.length; i < j; i++) {
            if (obj.hasOwnProperty(arguments[i])) {
                return true
            }
        }
        return false
    };
    var uniq = function(arr) {
        var len = arr.length;
        var set = {};
        for (var i = 0; i < len; i++) {
            set[arr[i]] = true
        }
        return objkeys(set)
    };
    var arbor_path = function() {
		return mw.config.get('wgScriptPath') + "/extensions/WorkflowTasks/js/lib/arbor/";
        /*var candidates = $("script").map(function(elt) {
            var src = $(this).attr("src");
            if (!src) {
                return
            }
            if (src.match(/arbor[^\/\.]*.js|dev.js/)) {
                return src.match(/.*\//) || "/"
            }
        });
        if (candidates.length > 0) {
            return candidates[0]
        } else {
            return null
        }*/
    };
    /*     kernel.js */
    var Kernel = function(pSystem) {
        var chrome_local_file = window.location.protocol == "file:" && navigator.userAgent.toLowerCase().indexOf("chrome") > -1;
        var USE_WORKER = (window.Worker !== undefined && !chrome_local_file);
        var _physics = null;
        var _tween = null;
        var _fpsWindow = [];
        _fpsWindow.last = new Date();
        var _screenInterval = null;
        var _attached = null;
        var _tickInterval = null;
        var _lastTick = null;
        var _paused = false;
        var that = {
            system: pSystem,
            tween: null,
            nodes: {},
            init: function() {
                if (typeof(Tween) != "undefined") {
                    _tween = Tween()
                } else {
                    if (typeof(arbor.Tween) != "undefined") {
                        _tween = arbor.Tween()
                    } else {
                        _tween = {
                            busy: function() {
                                return false
                            },
                            tick: function() {
                                return true
                            },
                            to: function() {
                                trace("Please include arbor-tween.js to enable tweens");
                                _tween.to = function() {};
                                return
                            }
                        }
                    }
                }
                that.tween = _tween;
                var params = pSystem.parameters();
                if (USE_WORKER) {
                    trace("using web workers");
                    _screenInterval = setInterval(that.screenUpdate, params.timeout);
                    _physics = new Worker(arbor_path() + "arbor.js");
                    _physics.onmessage = that.workerMsg;
                    _physics.onerror = function(e) {
                        trace("physics:", e)
                    };
                    _physics.postMessage({
                        type: "physics",
                        physics: objmerge(params, {
                            timeout: Math.ceil(params.timeout)
                        })
                    })
                } else {
                    trace("couldn't use web workers, be careful...");
                    _physics = Physics(params.dt, params.stiffness, params.repulsion, params.friction, that.system._updateGeometry);
                    that.start()
                }
                return that
            },
            graphChanged: function(changes) {
                if (USE_WORKER) {
                    _physics.postMessage({
                        type: "changes",
                        "changes": changes
                    })
                } else {
                    _physics._update(changes)
                }
                that.start()
            },
            particleModified: function(id, mods) {
                if (USE_WORKER) {
                    _physics.postMessage({
                        type: "modify",
                        id: id,
                        mods: mods
                    })
                } else {
                    _physics.modifyNode(id, mods)
                }
                that.start()
            },
            physicsModified: function(param) {
                if (!isNaN(param.timeout)) {
                    if (USE_WORKER) {
                        clearInterval(_screenInterval);
                        _screenInterval = setInterval(that.screenUpdate, param.timeout)
                    } else {
                        clearInterval(_tickInterval);
                        _tickInterval = null
                    }
                }
                if (USE_WORKER) {
                    _physics.postMessage({
                        type: "sys",
                        param: param
                    })
                } else {
                    _physics.modifyPhysics(param)
                }
                that.start()
            },
            workerMsg: function(e) {
                var type = e.data.type;
                if (type == "geometry") {
                    that.workerUpdate(e.data)
                } else {
                    trace("physics:", e.data)
                }
            },
            _lastPositions: null,
            workerUpdate: function(data) {
                that._lastPositions = data;
                that._lastBounds = data.bounds
            },
            _lastFrametime: new Date().valueOf(),
            _lastBounds: null,
            _currentRenderer: null,
            screenUpdate: function() {
                var now = new Date().valueOf();
                var shouldRedraw = false;
                if (that._lastPositions !== null) {
                    that.system._updateGeometry(that._lastPositions);
                    that._lastPositions = null;
                    shouldRedraw = true
                }
                if (_tween && _tween.busy()) {
                    shouldRedraw = true
                }
                if (that.system._updateBounds(that._lastBounds)) {
                    shouldRedraw = true
                }
                if (shouldRedraw) {
                    var render = that.system.renderer;
                    if (render !== undefined) {
                        if (render !== _attached) {
                            render.init(that.system);
                            _attached = render
                        }
                        if (_tween) {
                            _tween.tick()
                        }
                        render.redraw();
                        var prevFrame = _fpsWindow.last;
                        _fpsWindow.last = new Date();
                        _fpsWindow.push(_fpsWindow.last - prevFrame);
                        if (_fpsWindow.length > 50) {
                            _fpsWindow.shift()
                        }
                    }
                }
            },
            physicsUpdate: function() {
                if (_tween) {
                    _tween.tick()
                }
                _physics.tick();
                var stillActive = that.system._updateBounds();
                if (_tween && _tween.busy()) {
                    stillActive = true
                }
                var render = that.system.renderer;
                var now = new Date();
                var render = that.system.renderer;
                if (render !== undefined) {
                    if (render !== _attached) {
                        render.init(that.system);
                        _attached = render
                    }
                    render.redraw({
                        timestamp: now
                    })
                }
                var prevFrame = _fpsWindow.last;
                _fpsWindow.last = now;
                _fpsWindow.push(_fpsWindow.last - prevFrame);
                if (_fpsWindow.length > 50) {
                    _fpsWindow.shift()
                }
                var sysEnergy = _physics.systemEnergy();
                if ((sysEnergy.mean + sysEnergy.max) / 2 < 0.05) {
                    if (_lastTick === null) {
                        _lastTick = new Date().valueOf()
                    }
                    if (new Date().valueOf() - _lastTick > 1000) {
                        clearInterval(_tickInterval);
                        _tickInterval = null
                    } else {}
                } else {
                    _lastTick = null
                }
            },
            fps: function(newTargetFPS) {
                if (newTargetFPS !== undefined) {
                    var timeout = 1000 / Math.max(1, targetFps);
                    that.physicsModified({
                        timeout: timeout
                    })
                }
                var totInterv = 0;
                for (var i = 0, j = _fpsWindow.length; i < j; i++) {
                    totInterv += _fpsWindow[i]
                }
                var meanIntev = totInterv / Math.max(1, _fpsWindow.length);
                if (!isNaN(meanIntev)) {
                    return Math.round(1000 / meanIntev)
                } else {
                    return 0
                }
            },
            start: function(unpause) {
                if (_tickInterval !== null) {
                    return
                }
                if (_paused && !unpause) {
                    return
                }
                _paused = false;
                if (USE_WORKER) {
                    _physics.postMessage({
                        type: "start"
                    })
                } else {
                    _lastTick = null;
                    _tickInterval = setInterval(that.physicsUpdate, that.system.parameters().timeout)
                }
            },
            stop: function() {
                _paused = true;
                if (USE_WORKER) {
                    _physics.postMessage({
                        type: "stop"
                    })
                } else {
                    if (_tickInterval !== null) {
                        clearInterval(_tickInterval);
                        _tickInterval = null
                    }
                }
            }
        };
        return that.init()
    };
    /*      atoms.js */
    var Node = function(data) {
        this._id = _nextNodeId++;
        this.data = data || {};
        this._mass = (data.mass !== undefined) ? data.mass : 1;
        this._fixed = (data.fixed === true) ? true : false;
        this._p = new Point((typeof(data.x) == "number") ? data.x : null, (typeof(data.y) == "number") ? data.y : null);
        delete this.data.x;
        delete this.data.y;
        delete this.data.mass;
        delete this.data.fixed
    };
    var _nextNodeId = 1;
    var Edge = function(source, target, data) {
        this._id = _nextEdgeId--;
        this.source = source;
        this.target = target;
        this.length = (data.length !== undefined) ? data.length : 1;
        this.data = (data !== undefined) ? data : {};
        delete this.data.length
    };
    var _nextEdgeId = -1;
    var Particle = function(position, mass) {
        this.p = position;
        this.m = mass;
        this.v = new Point(0, 0);
        this.f = new Point(0, 0)
    };
    Particle.prototype.applyForce = function(force) {
        this.f = this.f.add(force.divide(this.m))
    };
    var Spring = function(point1, point2, length, k) {
        this.point1 = point1;
        this.point2 = point2;
        this.length = length;
        this.k = k
    };
    Spring.prototype.distanceToParticle = function(point) {
        var n = that.point2.p.subtract(that.point1.p).normalize().normal();
        var ac = point.p.subtract(that.point1.p);
        return Math.abs(ac.x * n.x + ac.y * n.y)
    };
    var Point = function(x, y) {
        if (x && x.hasOwnProperty("y")) {
            y = x.y;
            x = x.x
        }
        this.x = x;
        this.y = y
    };
    Point.random = function(radius) {
        radius = (radius !== undefined) ? radius : 5;
        return new Point(2 * radius * (Math.random() - 0.5), 2 * radius * (Math.random() - 0.5))
    };
    Point.prototype = {
        exploded: function() {
            return (isNaN(this.x) || isNaN(this.y))
        },
        add: function(v2) {
            return new Point(this.x + v2.x, this.y + v2.y)
        },
        subtract: function(v2) {
            return new Point(this.x - v2.x, this.y - v2.y)
        },
        multiply: function(n) {
            return new Point(this.x * n, this.y * n)
        },
        divide: function(n) {
            return new Point(this.x / n, this.y / n)
        },
        magnitude: function() {
            return Math.sqrt(this.x * this.x + this.y * this.y)
        },
        normal: function() {
            return new Point(-this.y, this.x)
        },
        normalize: function() {
            return this.divide(this.magnitude())
        }
    };
    /*     system.js */
    var ParticleSystem = function(repulsion, stiffness, friction, centerGravity, targetFps, dt, precision) {
        var _changes = [];
        var _notification = null;
        var _epoch = 0;
        var _screenSize = null;
        var _screenStep = 0.04;
        var _screenPadding = [20, 20, 20, 20];
        var _bounds = null;
        var _boundsTarget = null;
        if (typeof stiffness == "object") {
            var _p = stiffness;
            friction = _p.friction;
            repulsion = _p.repulsion;
            targetFps = _p.fps;
            dt = _p.dt;
            stiffness = _p.stiffness;
            centerGravity = _p.gravity;
            precision = _p.precision
        }
        friction = isNaN(friction) ? 0.5 : friction;
        repulsion = isNaN(repulsion) ? 1000 : repulsion;
        targetFps = isNaN(targetFps) ? 55 : targetFps;
        stiffness = isNaN(stiffness) ? 600 : stiffness;
        dt = isNaN(dt) ? 0.02 : dt;
        precision = isNaN(precision) ? 0.6 : precision;
        centerGravity = (centerGravity === true);
        var _systemTimeout = (targetFps !== undefined) ? 1000 / targetFps : 1000 / 50;
        var _parameters = {
            repulsion: repulsion,
            stiffness: stiffness,
            friction: friction,
            dt: dt,
            gravity: centerGravity,
            precision: precision,
            timeout: _systemTimeout
        };
        var _energy;
        var state = {
            renderer: null,
            tween: null,
            nodes: {},
            edges: {},
            adjacency: {},
            names: {},
            kernel: null
        };
        var that = {
            parameters: function(newParams) {
                if (newParams !== undefined) {
                    if (!isNaN(newParams.precision)) {
                        newParams.precision = Math.max(0, Math.min(1, newParams.precision))
                    }
                    $.each(_parameters, function(p, v) {
                        if (newParams[p] !== undefined) {
                            _parameters[p] = newParams[p]
                        }
                    });
                    state.kernel.physicsModified(newParams)
                }
                return _parameters
            },
            fps: function(newFPS) {
                if (newFPS === undefined) {
                    return state.kernel.fps()
                } else {
                    that.parameters({
                        timeout: 1000 / (newFPS || 50)
                    })
                }
            },
            start: function() {
                state.kernel.start()
            },
            stop: function() {
                state.kernel.stop()
            },
            addNode: function(name, data) {
                data = data || {};
                var priorNode = state.names[name];
                if (priorNode) {
                    priorNode.data = data;
                    return priorNode
                } else {
                    if (name != undefined) {
                        var x = (data.x != undefined && !isNaN(data.x)) ? data.x : null;
                        var y = (data.y != undefined && !isNaN(data.y)) ? data.y : null;
                        var fixed = (data.fixed) ? 1 : 0;
                        var node = new Node(data);
                        node.name = name;
                        state.names[name] = node;
                        state.nodes[node._id] = node;
                        _changes.push({
                            t: "addNode",
                            id: node._id,
                            m: node.mass,
                            x: x,
                            y: y,
                            f: fixed
                        });
                        that._notify();
                        return node
                    }
                }
            },
            pruneNode: function(nodeOrName) {
                var node = that.getNode(nodeOrName);
                if (typeof(state.nodes[node._id]) !== "undefined") {
                    delete state.nodes[node._id];
                    delete state.names[node.name]
                }
                $.each(state.edges, function(id, e) {
                    if (e.source._id === node._id || e.target._id === node._id) {
                        that.pruneEdge(e)
                    }
                });
                _changes.push({
                    t: "dropNode",
                    id: node._id
                });
                that._notify()
            },
            getNode: function(nodeOrName) {
                if (nodeOrName._id !== undefined) {
                    return nodeOrName
                } else {
                    if (typeof nodeOrName == "string" || typeof nodeOrName == "number") {
                        return state.names[nodeOrName]
                    }
                }
            },
            eachNode: function(callback) {
                $.each(state.nodes, function(id, n) {
                    if (n._p.x == null || n._p.y == null) {
                        return
                    }
                    var pt = (_screenSize !== null) ? that.toScreen(n._p) : n._p;
                    callback.call(that, n, pt)
                })
            },
            addEdge: function(source, target, data) {
                source = that.getNode(source) || that.addNode(source);
                target = that.getNode(target) || that.addNode(target);
                data = data || {};
                var edge = new Edge(source, target, data);
                var src = source._id;
                var dst = target._id;
                state.adjacency[src] = state.adjacency[src] || {};
                state.adjacency[src][dst] = state.adjacency[src][dst] || [];
                var exists = (state.adjacency[src][dst].length > 0);
                if (exists) {
                    $.extend(state.adjacency[src][dst].data, edge.data);
                    return
                } else {
                    state.edges[edge._id] = edge;
                    state.adjacency[src][dst].push(edge);
                    var len = (edge.length !== undefined) ? edge.length : 1;
                    _changes.push({
                        t: "addSpring",
                        id: edge._id,
                        fm: src,
                        to: dst,
                        l: len
                    });
                    that._notify()
                }
                return edge
            },
            pruneEdge: function(edge) {
                _changes.push({
                    t: "dropSpring",
                    id: edge._id
                });
                delete state.edges[edge._id];
                for (var x in state.adjacency) {
                    for (var y in state.adjacency[x]) {
                        var edges = state.adjacency[x][y];
                        for (var j = edges.length - 1; j >= 0; j--) {
                            if (state.adjacency[x][y][j]._id === edge._id) {
                                state.adjacency[x][y].splice(j, 1)
                            }
                        }
                    }
                }
                that._notify()
            },
            getEdges: function(node1, node2) {
                node1 = that.getNode(node1);
                node2 = that.getNode(node2);
                if (!node1 || !node2) {
                    return []
                }
                if (typeof(state.adjacency[node1._id]) !== "undefined" && typeof(state.adjacency[node1._id][node2._id]) !== "undefined") {
                    return state.adjacency[node1._id][node2._id]
                }
                return []
            },
            getEdgesFrom: function(node) {
                node = that.getNode(node);
                if (!node) {
                    return []
                }
                if (typeof(state.adjacency[node._id]) !== "undefined") {
                    var nodeEdges = [];
                    $.each(state.adjacency[node._id], function(id, subEdges) {
                        nodeEdges = nodeEdges.concat(subEdges)
                    });
                    return nodeEdges
                }
                return []
            },
            getEdgesTo: function(node) {
                node = that.getNode(node);
                if (!node) {
                    return []
                }
                var nodeEdges = [];
                $.each(state.edges, function(edgeId, edge) {
                    if (edge.target == node) {
                        nodeEdges.push(edge)
                    }
                });
                return nodeEdges
            },
            eachEdge: function(callback) {
                $.each(state.edges, function(id, e) {
                    var p1 = state.nodes[e.source._id]._p;
                    var p2 = state.nodes[e.target._id]._p;
                    if (p1.x == null || p2.x == null) {
                        return
                    }
                    p1 = (_screenSize !== null) ? that.toScreen(p1) : p1;
                    p2 = (_screenSize !== null) ? that.toScreen(p2) : p2;
                    if (p1 && p2) {
                        callback.call(that, e, p1, p2)
                    }
                })
            },
            prune: function(callback) {
                var changes = {
                    dropped: {
                        nodes: [],
                        edges: []
                    }
                };
                if (callback === undefined) {
                    $.each(state.nodes, function(id, node) {
                        changes.dropped.nodes.push(node);
                        that.pruneNode(node)
                    })
                } else {
                    that.eachNode(function(node) {
                        var drop = callback.call(that, node, {
                            from: that.getEdgesFrom(node),
                            to: that.getEdgesTo(node)
                        });
                        if (drop) {
                            changes.dropped.nodes.push(node);
                            that.pruneNode(node)
                        }
                    })
                }
                return changes
            },
            graft: function(branch) {
                var changes = {
                    added: {
                        nodes: [],
                        edges: []
                    }
                };
                if (branch.nodes) {
                    $.each(branch.nodes, function(name, nodeData) {
                        var oldNode = that.getNode(name);
                        if (oldNode) {
                            oldNode.data = nodeData
                        } else {
                            changes.added.nodes.push(that.addNode(name, nodeData))
                        }
                        state.kernel.start()
                    })
                }
                if (branch.edges) {
                    $.each(branch.edges, function(src, dsts) {
                        var srcNode = that.getNode(src);
                        if (!srcNode) {
                            changes.added.nodes.push(that.addNode(src, {}))
                        }
                        $.each(dsts, function(dst, edgeData) {
                            var dstNode = that.getNode(dst);
                            if (!dstNode) {
                                changes.added.nodes.push(that.addNode(dst, {}))
                            }
                            var oldEdges = that.getEdges(src, dst);
                            if (oldEdges.length > 0) {
                                oldEdges[0].data = edgeData
                            } else {
                                changes.added.edges.push(that.addEdge(src, dst, edgeData))
                            }
                        })
                    })
                }
                return changes
            },
            merge: function(branch) {
                var changes = {
                    added: {
                        nodes: [],
                        edges: []
                    },
                    dropped: {
                        nodes: [],
                        edges: []
                    }
                };
                $.each(state.edges, function(id, edge) {
                    if ((branch.edges[edge.source.name] === undefined || branch.edges[edge.source.name][edge.target.name] === undefined)) {
                        that.pruneEdge(edge);
                        changes.dropped.edges.push(edge)
                    }
                });
                var prune_changes = that.prune(function(node, edges) {
                    if (branch.nodes[node.name] === undefined) {
                        changes.dropped.nodes.push(node);
                        return true
                    }
                });
                var graft_changes = that.graft(branch);
                changes.added.nodes = changes.added.nodes.concat(graft_changes.added.nodes);
                changes.added.edges = changes.added.edges.concat(graft_changes.added.edges);
                changes.dropped.nodes = changes.dropped.nodes.concat(prune_changes.dropped.nodes);
                changes.dropped.edges = changes.dropped.edges.concat(prune_changes.dropped.edges);
                return changes
            },
            tweenNode: function(nodeOrName, dur, to) {
                var node = that.getNode(nodeOrName);
                if (node) {
                    state.tween.to(node, dur, to)
                }
            },
            tweenEdge: function(a, b, c, d) {
                if (d === undefined) {
                    that._tweenEdge(a, b, c)
                } else {
                    var edges = that.getEdges(a, b);
                    $.each(edges, function(i, edge) {
                        that._tweenEdge(edge, c, d)
                    })
                }
            },
            _tweenEdge: function(edge, dur, to) {
                if (edge && edge._id !== undefined) {
                    state.tween.to(edge, dur, to)
                }
            },
            _updateGeometry: function(e) {
                if (e != undefined) {
                    var stale = (e.epoch < _epoch);
                    _energy = e.energy;
                    var pts = e.geometry;
                    if (pts !== undefined) {
                        for (var i = 0, j = pts.length / 3; i < j; i++) {
                            var id = pts[3 * i];
                            if (stale && state.nodes[id] == undefined) {
                                continue
                            }
                            state.nodes[id]._p.x = pts[3 * i + 1];
                            state.nodes[id]._p.y = pts[3 * i + 2]
                        }
                    }
                }
            },
            screen: function(opts) {
                if (opts == undefined) {
                    return {
                        size: (_screenSize) ? objcopy(_screenSize) : undefined,
                        padding: _screenPadding.concat(),
                        step: _screenStep
                    }
                }
                if (opts.size !== undefined) {
                    that.screenSize(opts.size.width, opts.size.height)
                }
                if (!isNaN(opts.step)) {
                    that.screenStep(opts.step)
                }
                if (opts.padding !== undefined) {
                    that.screenPadding(opts.padding)
                }
            },
            screenSize: function(canvasWidth, canvasHeight) {
                _screenSize = {
                    width: canvasWidth,
                    height: canvasHeight
                };
                that._updateBounds()
            },
            screenPadding: function(t, r, b, l) {
                if ($.isArray(t)) {
                    trbl = t
                } else {
                    trbl = [t, r, b, l]
                }
                var top = trbl[0];
                var right = trbl[1];
                var bot = trbl[2];
                if (right === undefined) {
                    trbl = [top, top, top, top]
                } else {
                    if (bot == undefined) {
                        trbl = [top, right, top, right]
                    }
                }
                _screenPadding = trbl
            },
            screenStep: function(stepsize) {
                _screenStep = stepsize
            },
            toScreen: function(p) {
                if (!_bounds || !_screenSize) {
                    return
                }
                var _padding = _screenPadding || [0, 0, 0, 0];
                var size = _bounds.bottomright.subtract(_bounds.topleft);
                var sx = _padding[3] + p.subtract(_bounds.topleft).divide(size.x).x * (_screenSize.width - (_padding[1] + _padding[3]));
                var sy = _padding[0] + p.subtract(_bounds.topleft).divide(size.y).y * (_screenSize.height - (_padding[0] + _padding[2]));
                return arbor.Point(sx, sy)
            },
            fromScreen: function(s) {
                if (!_bounds || !_screenSize) {
                    return
                }
                var _padding = _screenPadding || [0, 0, 0, 0];
                var size = _bounds.bottomright.subtract(_bounds.topleft);
                var px = (s.x - _padding[3]) / (_screenSize.width - (_padding[1] + _padding[3])) * size.x + _bounds.topleft.x;
                var py = (s.y - _padding[0]) / (_screenSize.height - (_padding[0] + _padding[2])) * size.y + _bounds.topleft.y;
                return arbor.Point(px, py)
            },
            _updateBounds: function(newBounds) {
                if (_screenSize === null) {
                    return
                }
                if (newBounds) {
                    _boundsTarget = newBounds
                } else {
                    _boundsTarget = that.bounds()
                }
                var bottomright = new Point(_boundsTarget.bottomright.x, _boundsTarget.bottomright.y);
                var topleft = new Point(_boundsTarget.topleft.x, _boundsTarget.topleft.y);
                var dims = bottomright.subtract(topleft);
                var center = topleft.add(dims.divide(2));
                var MINSIZE = 4;
                var size = new Point(Math.max(dims.x, MINSIZE), Math.max(dims.y, MINSIZE));
                _boundsTarget.topleft = center.subtract(size.divide(2));
                _boundsTarget.bottomright = center.add(size.divide(2));
                if (!_bounds) {
                    if ($.isEmptyObject(state.nodes)) {
                        return false
                    }
                    _bounds = _boundsTarget;
                    return true
                }
                var stepSize = _screenStep;
                _newBounds = {
                    bottomright: _bounds.bottomright.add(_boundsTarget.bottomright.subtract(_bounds.bottomright).multiply(stepSize)),
                    topleft: _bounds.topleft.add(_boundsTarget.topleft.subtract(_bounds.topleft).multiply(stepSize))
                };
                var diff = new Point(_bounds.topleft.subtract(_newBounds.topleft).magnitude(), _bounds.bottomright.subtract(_newBounds.bottomright).magnitude());
                if (diff.x * _screenSize.width > 1 || diff.y * _screenSize.height > 1) {
                    _bounds = _newBounds;
                    return true
                } else {
                    return false
                }
            },
            energy: function() {
                return _energy
            },
            bounds: function() {
                var bottomright = null;
                var topleft = null;
                $.each(state.nodes, function(id, node) {
                    if (!bottomright) {
                        bottomright = new Point(node._p);
                        topleft = new Point(node._p);
                        return
                    }
                    var point = node._p;
                    if (point.x === null || point.y === null) {
                        return
                    }
                    if (point.x > bottomright.x) {
                        bottomright.x = point.x
                    }
                    if (point.y > bottomright.y) {
                        bottomright.y = point.y
                    }
                    if (point.x < topleft.x) {
                        topleft.x = point.x
                    }
                    if (point.y < topleft.y) {
                        topleft.y = point.y
                    }
                });
                if (bottomright && topleft) {
                    return {
                        bottomright: bottomright,
                        topleft: topleft
                    }
                } else {
                    return {
                        topleft: new Point(-1, -1),
                        bottomright: new Point(1, 1)
                    }
                }
            },
            nearest: function(pos) {
                if (_screenSize !== null) {
                    pos = that.fromScreen(pos)
                }
                var min = {
                    node: null,
                    point: null,
                    distance: null
                };
                var t = that;
                $.each(state.nodes, function(id, node) {
                    var pt = node._p;
                    if (pt.x === null || pt.y === null) {
                        return
                    }
                    var distance = pt.subtract(pos).magnitude();
                    if (min.distance === null || distance < min.distance) {
                        min = {
                            node: node,
                            point: pt,
                            distance: distance
                        };
                        if (_screenSize !== null) {
                            min.screenPoint = that.toScreen(pt)
                        }
                    }
                });
                if (min.node) {
                    if (_screenSize !== null) {
                        min.distance = that.toScreen(min.node.p).subtract(that.toScreen(pos)).magnitude()
                    }
                    return min
                } else {
                    return null
                }
            },
            _notify: function() {
                if (_notification === null) {
                    _epoch++
                } else {
                    clearTimeout(_notification)
                }
                _notification = setTimeout(that._synchronize, 20)
            },
            _synchronize: function() {
                if (_changes.length > 0) {
                    state.kernel.graphChanged(_changes);
                    _changes = [];
                    _notification = null
                }
            },
        };
        state.kernel = Kernel(that);
        state.tween = state.kernel.tween || null;
        Node.prototype.__defineGetter__("p", function() {
            var self = this;
            var roboPoint = {};
            roboPoint.__defineGetter__("x", function() {
                return self._p.x
            });
            roboPoint.__defineSetter__("x", function(newX) {
                state.kernel.particleModified(self._id, {
                    x: newX
                })
            });
            roboPoint.__defineGetter__("y", function() {
                return self._p.y
            });
            roboPoint.__defineSetter__("y", function(newY) {
                state.kernel.particleModified(self._id, {
                    y: newY
                })
            });
            roboPoint.__proto__ = Point.prototype;
            return roboPoint
        });
        Node.prototype.__defineSetter__("p", function(newP) {
            this._p.x = newP.x;
            this._p.y = newP.y;
            state.kernel.particleModified(this._id, {
                x: newP.x,
                y: newP.y
            })
        });
        Node.prototype.__defineGetter__("mass", function() {
            return this._mass
        });
        Node.prototype.__defineSetter__("mass", function(newM) {
            this._mass = newM;
            state.kernel.particleModified(this._id, {
                m: newM
            })
        });
        Node.prototype.__defineSetter__("tempMass", function(newM) {
            state.kernel.particleModified(this._id, {
                _m: newM
            })
        });
        Node.prototype.__defineGetter__("fixed", function() {
            return this._fixed
        });
        Node.prototype.__defineSetter__("fixed", function(isFixed) {
            this._fixed = isFixed;
            state.kernel.particleModified(this._id, {
                f: isFixed ? 1 : 0
            })
        });
        return that
    };
    /* barnes-hut.js */
    var BarnesHutTree = function() {
        var _branches = [];
        var _branchCtr = 0;
        var _root = null;
        var _theta = 0.5;
        var that = {
            init: function(topleft, bottomright, theta) {
                _theta = theta;
                _branchCtr = 0;
                _root = that._newBranch();
                _root.origin = topleft;
                _root.size = bottomright.subtract(topleft)
            },
            insert: function(newParticle) {
                var node = _root;
                var queue = [newParticle];
                while (queue.length) {
                    var particle = queue.shift();
                    var p_mass = particle._m || particle.m;
                    var p_quad = that._whichQuad(particle, node);
                    if (node[p_quad] === undefined) {
                        node[p_quad] = particle;
                        node.mass += p_mass;
                        if (node.p) {
                            node.p = node.p.add(particle.p.multiply(p_mass))
                        } else {
                            node.p = particle.p.multiply(p_mass)
                        }
                    } else {
                        if ("origin" in node[p_quad]) {
                            node.mass += (p_mass);
                            if (node.p) {
                                node.p = node.p.add(particle.p.multiply(p_mass))
                            } else {
                                node.p = particle.p.multiply(p_mass)
                            }
                            node = node[p_quad];
                            queue.unshift(particle)
                        } else {
                            var branch_size = node.size.divide(2);
                            var branch_origin = new Point(node.origin);
                            if (p_quad[0] == "s") {
                                branch_origin.y += branch_size.y
                            }
                            if (p_quad[1] == "e") {
                                branch_origin.x += branch_size.x
                            }
                            var oldParticle = node[p_quad];
                            node[p_quad] = that._newBranch();
                            node[p_quad].origin = branch_origin;
                            node[p_quad].size = branch_size;
                            node.mass = p_mass;
                            node.p = particle.p.multiply(p_mass);
                            node = node[p_quad];
                            if (oldParticle.p.x === particle.p.x && oldParticle.p.y === particle.p.y) {
                                var x_spread = branch_size.x * 0.08;
                                var y_spread = branch_size.y * 0.08;
                                oldParticle.p.x = Math.min(branch_origin.x + branch_size.x, Math.max(branch_origin.x, oldParticle.p.x - x_spread / 2 + Math.random() * x_spread));
                                oldParticle.p.y = Math.min(branch_origin.y + branch_size.y, Math.max(branch_origin.y, oldParticle.p.y - y_spread / 2 + Math.random() * y_spread))
                            }
                            queue.push(oldParticle);
                            queue.unshift(particle)
                        }
                    }
                }
            },
            applyForces: function(particle, repulsion) {
                var queue = [_root];
                while (queue.length) {
                    node = queue.shift();
                    if (node === undefined) {
                        continue
                    }
                    if (particle === node) {
                        continue
                    }
                    if ("f" in node) {
                        var d = particle.p.subtract(node.p);
                        var distance = Math.max(1, d.magnitude());
                        var direction = ((d.magnitude() > 0) ? d : Point.random(1)).normalize();
                        particle.applyForce(direction.multiply(repulsion * (node._m || node.m)).divide(distance * distance))
                    } else {
                        var dist = particle.p.subtract(node.p.divide(node.mass)).magnitude();
                        var size = Math.sqrt(node.size.x * node.size.y);
                        if (size / dist > _theta) {
                            queue.push(node.ne);
                            queue.push(node.nw);
                            queue.push(node.se);
                            queue.push(node.sw)
                        } else {
                            var d = particle.p.subtract(node.p.divide(node.mass));
                            var distance = Math.max(1, d.magnitude());
                            var direction = ((d.magnitude() > 0) ? d : Point.random(1)).normalize();
                            particle.applyForce(direction.multiply(repulsion * (node.mass)).divide(distance * distance))
                        }
                    }
                }
            },
            _whichQuad: function(particle, node) {
                if (particle.p.exploded()) {
                    return null
                }
                var particle_p = particle.p.subtract(node.origin);
                var halfsize = node.size.divide(2);
                if (particle_p.y < halfsize.y) {
                    if (particle_p.x < halfsize.x) {
                        return "nw"
                    } else {
                        return "ne"
                    }
                } else {
                    if (particle_p.x < halfsize.x) {
                        return "sw"
                    } else {
                        return "se"
                    }
                }
            },
            _newBranch: function() {
                if (_branches[_branchCtr]) {
                    var branch = _branches[_branchCtr];
                    branch.ne = branch.nw = branch.se = branch.sw = undefined;
                    branch.mass = 0;
                    delete branch.p
                } else {
                    branch = {
                        origin: null,
                        size: null,
                        nw: undefined,
                        ne: undefined,
                        sw: undefined,
                        se: undefined,
                        mass: 0
                    };
                    _branches[_branchCtr] = branch
                }
                _branchCtr++;
                return branch
            }
        };
        return that
    };
    /*    physics.js */
    var Physics = function(dt, stiffness, repulsion, friction, updateFn) {
        var bhTree = BarnesHutTree();
        var active = {
            particles: {},
            springs: {}
        };
        var free = {
            particles: {}
        };
        var particles = [];
        var springs = [];
        var _epoch = 0;
        var _energy = {
            sum: 0,
            max: 0,
            mean: 0
        };
        var _bounds = {
            topleft: new Point(-1, -1),
            bottomright: new Point(1, 1)
        };
        var SPEED_LIMIT = 1000;
        var that = {
            stiffness: (stiffness !== undefined) ? stiffness : 1000,
            repulsion: (repulsion !== undefined) ? repulsion : 600,
            friction: (friction !== undefined) ? friction : 0.3,
            gravity: false,
            dt: (dt !== undefined) ? dt : 0.02,
            theta: 0.4,
            init: function() {
                return that
            },
            modifyPhysics: function(param) {
                $.each(["stiffness", "repulsion", "friction", "gravity", "dt", "precision"], function(i, p) {
                    if (param[p] !== undefined) {
                        if (p == "precision") {
                            that.theta = 1 - param[p];
                            return
                        }
                        that[p] = param[p];
                        if (p == "stiffness") {
                            var stiff = param[p];
                            $.each(active.springs, function(id, spring) {
                                spring.k = stiff
                            })
                        }
                    }
                })
            },
            addNode: function(c) {
                var id = c.id;
                var mass = c.m;
                var w = _bounds.bottomright.x - _bounds.topleft.x;
                var h = _bounds.bottomright.y - _bounds.topleft.y;
                var randomish_pt = new Point((c.x != null) ? c.x : _bounds.topleft.x + w * Math.random(), (c.y != null) ? c.y : _bounds.topleft.y + h * Math.random());
                active.particles[id] = new Particle(randomish_pt, mass);
                active.particles[id].connections = 0;
                active.particles[id].fixed = (c.f === 1);
                free.particles[id] = active.particles[id];
                particles.push(active.particles[id])
            },
            dropNode: function(c) {
                var id = c.id;
                var dropping = active.particles[id];
                var idx = $.inArray(dropping, particles);
                if (idx > -1) {
                    particles.splice(idx, 1)
                }
                delete active.particles[id];
                delete free.particles[id]
            },
            modifyNode: function(id, mods) {
                if (id in active.particles) {
                    var pt = active.particles[id];
                    if ("x" in mods) {
                        pt.p.x = mods.x
                    }
                    if ("y" in mods) {
                        pt.p.y = mods.y
                    }
                    if ("m" in mods) {
                        pt.m = mods.m
                    }
                    if ("f" in mods) {
                        pt.fixed = (mods.f === 1)
                    }
                    if ("_m" in mods) {
                        if (pt._m === undefined) {
                            pt._m = pt.m
                        }
                        pt.m = mods._m
                    }
                }
            },
            addSpring: function(c) {
                var id = c.id;
                var length = c.l;
                var from = active.particles[c.fm];
                var to = active.particles[c.to];
                if (from !== undefined && to !== undefined) {
                    active.springs[id] = new Spring(from, to, length, that.stiffness);
                    springs.push(active.springs[id]);
                    from.connections++;
                    to.connections++;
                    delete free.particles[c.fm];
                    delete free.particles[c.to]
                }
            },
            dropSpring: function(c) {
                var id = c.id;
                var dropping = active.springs[id];
                dropping.point1.connections--;
                dropping.point2.connections--;
                var idx = $.inArray(dropping, springs);
                if (idx > -1) {
                    springs.splice(idx, 1)
                }
                delete active.springs[id]
            },
            _update: function(changes) {
                _epoch++;
                $.each(changes, function(i, c) {
                    if (c.t in that) {
                        that[c.t](c)
                    }
                });
                return _epoch
            },
            tick: function() {
                that.tendParticles();
                that.eulerIntegrator(that.dt);
                that.tock()
            },
            tock: function() {
                var coords = [];
                $.each(active.particles, function(id, pt) {
                    coords.push(id);
                    coords.push(pt.p.x);
                    coords.push(pt.p.y)
                });
                if (updateFn) {
                    updateFn({
                        geometry: coords,
                        epoch: _epoch,
                        energy: _energy,
                        bounds: _bounds
                    })
                }
            },
            tendParticles: function() {
                $.each(active.particles, function(id, pt) {
                    if (pt._m !== undefined) {
                        if (Math.abs(pt.m - pt._m) < 1) {
                            pt.m = pt._m;
                            delete pt._m
                        } else {
                            pt.m *= 0.98
                        }
                    }
                    pt.v.x = pt.v.y = 0
                })
            },
            eulerIntegrator: function(dt) {
                if (that.repulsion > 0) {
                    if (that.theta > 0) {
                        that.applyBarnesHutRepulsion()
                    } else {
                        that.applyBruteForceRepulsion()
                    }
                }
                if (that.stiffness > 0) {
                    that.applySprings()
                }
                that.applyCenterDrift();
                if (that.gravity) {
                    that.applyCenterGravity()
                }
                that.updateVelocity(dt);
                that.updatePosition(dt)
            },
            applyBruteForceRepulsion: function() {
                $.each(active.particles, function(id1, point1) {
                    $.each(active.particles, function(id2, point2) {
                        if (point1 !== point2) {
                            var d = point1.p.subtract(point2.p);
                            var distance = Math.max(1, d.magnitude());
                            var direction = ((d.magnitude() > 0) ? d : Point.random(1)).normalize();
                            point1.applyForce(direction.multiply(that.repulsion * (point2._m || point2.m) * 0.5).divide(distance * distance * 0.5));
                            point2.applyForce(direction.multiply(that.repulsion * (point1._m || point1.m) * 0.5).divide(distance * distance * -0.5))
                        }
                    })
                })
            },
            applyBarnesHutRepulsion: function() {
                if (!_bounds.topleft || !_bounds.bottomright) {
                    return
                }
                var bottomright = new Point(_bounds.bottomright);
                var topleft = new Point(_bounds.topleft);
                bhTree.init(topleft, bottomright, that.theta);
                $.each(active.particles, function(id, particle) {
                    bhTree.insert(particle)
                });
                $.each(active.particles, function(id, particle) {
                    bhTree.applyForces(particle, that.repulsion)
                })
            },
            applySprings: function() {
                $.each(active.springs, function(id, spring) {
                    var d = spring.point2.p.subtract(spring.point1.p);
                    var displacement = spring.length - d.magnitude();
                    var direction = ((d.magnitude() > 0) ? d : Point.random(1)).normalize();
                    spring.point1.applyForce(direction.multiply(spring.k * displacement * -0.5));
                    spring.point2.applyForce(direction.multiply(spring.k * displacement * 0.5))
                })
            },
            applyCenterDrift: function() {
                var numParticles = 0;
                var centroid = new Point(0, 0);
                $.each(active.particles, function(id, point) {
                    centroid.add(point.p);
                    numParticles++
                });
                if (numParticles == 0) {
                    return
                }
                var correction = centroid.divide(-numParticles);
                $.each(active.particles, function(id, point) {
                    point.applyForce(correction)
                })
            },
            applyCenterGravity: function() {
                $.each(active.particles, function(id, point) {
                    var direction = point.p.multiply(-1);
                    point.applyForce(direction.multiply(that.repulsion / 100))
                })
            },
            updateVelocity: function(timestep) {
                $.each(active.particles, function(id, point) {
                    if (point.fixed) {
                        point.v = new Point(0, 0);
                        point.f = new Point(0, 0);
                        return
                    }
                    var was = point.v.magnitude();
                    point.v = point.v.add(point.f.multiply(timestep)).multiply(1 - that.friction);
                    point.f.x = point.f.y = 0;
                    var speed = point.v.magnitude();
                    if (speed > SPEED_LIMIT) {
                        point.v = point.v.divide(speed * speed)
                    }
                })
            },
            updatePosition: function(timestep) {
                var sum = 0,
                    max = 0,
                    n = 0;
                var bottomright = null;
                var topleft = null;
                $.each(active.particles, function(i, point) {
                    point.p = point.p.add(point.v.multiply(timestep));
                    var speed = point.v.magnitude();
                    var e = speed * speed;
                    sum += e;
                    max = Math.max(e, max);
                    n++;
                    if (!bottomright) {
                        bottomright = new Point(point.p.x, point.p.y);
                        topleft = new Point(point.p.x, point.p.y);
                        return
                    }
                    var pt = point.p;
                    if (pt.x === null || pt.y === null) {
                        return
                    }
                    if (pt.x > bottomright.x) {
                        bottomright.x = pt.x
                    }
                    if (pt.y > bottomright.y) {
                        bottomright.y = pt.y
                    }
                    if (pt.x < topleft.x) {
                        topleft.x = pt.x
                    }
                    if (pt.y < topleft.y) {
                        topleft.y = pt.y
                    }
                });
                _energy = {
                    sum: sum,
                    max: max,
                    mean: sum / n,
                    n: n
                };
                _bounds = {
                    topleft: topleft || new Point(-1, -1),
                    bottomright: bottomright || new Point(1, 1)
                }
            },
            systemEnergy: function(timestep) {
                return _energy
            }
        };
        return that.init()
    };
    var _nearParticle = function(center_pt, r) {
        var r = r || 0;
        var x = center_pt.x;
        var y = center_pt.y;
        var d = r * 2;
        return new Point(x - r + Math.random() * d, y - r + Math.random() * d)
    };

    // if called as a worker thread, set up a run loop for the Physics object and bail out
    if (typeof(window) == 'undefined') return (function() {
        /* hermetic.js */
        $ = {
            each: function(obj, callback) {
                if ($.isArray(obj)) {
                    for (var i = 0, j = obj.length; i < j; i++) {
                        callback(i, obj[i])
                    }
                } else {
                    for (var k in obj) {
                        callback(k, obj[k])
                    }
                }
            },
            map: function(arr, fn) {
                var out = [];
                $.each(arr, function(i, elt) {
                    var result = fn(elt);
                    if (result !== undefined) {
                        out.push(result)
                    }
                });
                return out
            },
            extend: function(dst, src) {
                if (typeof src != "object") {
                    return dst
                }
                for (var k in src) {
                    if (src.hasOwnProperty(k)) {
                        dst[k] = src[k]
                    }
                }
                return dst
            },
            isArray: function(obj) {
                if (!obj) {
                    return false
                }
                return (obj.constructor.toString().indexOf("Array") != -1)
            },
            inArray: function(elt, arr) {
                for (var i = 0, j = arr.length; i < j; i++) {
                    if (arr[i] === elt) {
                        return i
                    }
                }
                return -1
            },
            isEmptyObject: function(obj) {
                if (typeof obj !== "object") {
                    return false
                }
                var isEmpty = true;
                $.each(obj, function(k, elt) {
                    isEmpty = false
                });
                return isEmpty
            },
        };
        /*     worker.js */
        var PhysicsWorker = function() {
            var _timeout = 20;
            var _physics = null;
            var _physicsInterval = null;
            var _lastTick = null;
            var times = [];
            var last = new Date().valueOf();
            var that = {
                init: function(param) {
                    that.timeout(param.timeout);
                    _physics = Physics(param.dt, param.stiffness, param.repulsion, param.friction, that.tock);
                    return that
                },
                timeout: function(newTimeout) {
                    if (newTimeout != _timeout) {
                        _timeout = newTimeout;
                        if (_physicsInterval !== null) {
                            that.stop();
                            that.go()
                        }
                    }
                },
                go: function() {
                    if (_physicsInterval !== null) {
                        return
                    }
                    _lastTick = null;
                    _physicsInterval = setInterval(that.tick, _timeout)
                },
                stop: function() {
                    if (_physicsInterval === null) {
                        return
                    }
                    clearInterval(_physicsInterval);
                    _physicsInterval = null
                },
                tick: function() {
                    _physics.tick();
                    var sysEnergy = _physics.systemEnergy();
                    if ((sysEnergy.mean + sysEnergy.max) / 2 < 0.05) {
                        if (_lastTick === null) {
                            _lastTick = new Date().valueOf()
                        }
                        if (new Date().valueOf() - _lastTick > 1000) {
                            that.stop()
                        } else {}
                    } else {
                        _lastTick = null
                    }
                },
                tock: function(sysData) {
                    sysData.type = "geometry";
                    postMessage(sysData)
                },
                modifyNode: function(id, mods) {
                    _physics.modifyNode(id, mods);
                    that.go()
                },
                modifyPhysics: function(param) {
                    _physics.modifyPhysics(param)
                },
                update: function(changes) {
                    var epoch = _physics._update(changes)
                }
            };
            return that
        };
        var physics = PhysicsWorker();
        onmessage = function(e) {
            if (!e.data.type) {
                postMessage("¿kérnèl?");
                return
            }
            if (e.data.type == "physics") {
                var param = e.data.physics;
                physics.init(e.data.physics);
                return
            }
            switch (e.data.type) {
                case "modify":
                    physics.modifyNode(e.data.id, e.data.mods);
                    break;
                case "changes":
                    physics.update(e.data.changes);
                    physics.go();
                    break;
                case "start":
                    physics.go();
                    break;
                case "stop":
                    physics.stop();
                    break;
                case "sys":
                    var param = e.data.param || {};
                    if (!isNaN(param.timeout)) {
                        physics.timeout(param.timeout)
                    }
                    physics.modifyPhysics(param);
                    physics.go();
                    break
            }
        };
    })()


    arbor = (typeof(arbor) !== 'undefined') ? arbor : {}
    $.extend(arbor, {
        // object constructors (don't use ⿿new⿿, just call them)
        ParticleSystem: ParticleSystem,
        Point: function(x, y) {
            return new Point(x, y)
        },

        // immutable object with useful methods
        etc: {
            trace: trace, // ƿ(msg) -> safe console logging
            dirname: dirname, // ƿ(path) -> leading part of path
            basename: basename, // ƿ(path) -> trailing part of path
            ordinalize: ordinalize, // ƿ(num) -> abbrev integers (and add commas)
            objcopy: objcopy, // ƿ(old) -> clone an object
            objcmp: objcmp, // ƿ(a, b, strict_ordering) -> t/f comparison
            objkeys: objkeys, // ƿ(obj) -> array of all keys in obj
            objmerge: objmerge, // ƿ(dst, src) -> like $.extend but non-destructive
            uniq: uniq, // ƿ(arr) -> array of unique items in arr
            arbor_path: arbor_path, // ƿ() -> guess the directory of the lib code
        }
    })

})(this.jQuery);
