var WTSubTasks = function(title, expapi, util, api, baseurl, $tree, menu) {
	this.baseurl = baseurl;
	this.title = title;
	this.util = util;
	this.api = api;	
	this.expapi = expapi;
	this.expapi.subscribe(this);
	this.$tree = $tree; 	
	this.menu = menu;
};

WTSubTasks.prototype.display = function() {
	var me = this;
	me.ctx = me.expapi.context();	
	
	var pleft = me.ctx.parent ? '17px' : '4px';
	me.$tree.css('padding-left', pleft);
	
	me.$tree.jstree("destroy");	
	me.$tree.jstree({
		  "core" : {
			    "animation" : 0,
			    "check_callback" : true,
			    'data': WTExplorerEvents.appendNewTaskNode(this.ctx.explorer, me.baseurl, me.expapi)
		  },
		  "plugins": ["contextmenu"],
		  "contextmenu": { "items": me.menu.subexplorerOptions }
	});
	
	me.$tree.on("before_open.jstree", function (e, data) {
		WTTracker.track({
			component: WTTracker.c.subtasks,
			actiontype: WTTracker.t.nav,
			action: 'expand task',
			taskId: data.node.id
		});
	});
	me.$tree.on("close_node.jstree", function (e, data) {
		WTTracker.track({
			component: WTTracker.c.subtasks,
			actiontype: WTTracker.t.nav,
			action: 'close task',
			taskId: data.node.id
		});
	});	
	
	WTExplorerEvents.attachOpenTaskEvent(me.$tree, me.baseurl, function(taskId, baseurl){
		WTTracker.track({
			component: WTTracker.c.subtasks,
			actiontype: WTTracker.t.nav,
			action: 'open task',
			taskId: taskId,
			callback: function(){
				document.location.href = baseurl+'/index.php/'+taskId;
			}
		});
	});
	WTExplorerEvents.attachAddTaskEvent(me.$tree, me.expapi, me.api, me.title, me.baseurl, false, null, function(value){
        WTTracker.track({
            component: WTTracker.c.subtasks,
            actiontype: WTTracker.t.add,
            action: 'add subtask',
            taskId: me.title,
            value: value
        });
	});
};

WTSubTasks.prototype.notify = function() {
	this.display();
};


WTSubTasks.prototype.lock = function(message) {
	this.$tree.mask(lpMsg(message));
};
