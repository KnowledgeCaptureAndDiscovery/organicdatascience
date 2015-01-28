var WTExplorerMenu = function(title, api, expapi, baseurl) {
	WTExplorerMenu.instance = this;	
	this.baseurl = baseurl;
	this.expapi = expapi;
	this.title = title;
	this.api = api;	
};

WTExplorerMenu.prototype.explorerOptions = function(node) {
	return WTExplorerMenu.instance.options(node, WTTracker.c.explorer); 	
};

WTExplorerMenu.prototype.subexplorerOptions = function(node) {
	return WTExplorerMenu.instance.options(node, WTTracker.c.subtasks); 	
};

WTExplorerMenu.prototype.options = function(node, trackingComponent) {
	var me = WTExplorerMenu.instance;
	me.menu = {};
	if(node.id != 'AddNewSubTask' && me.expapi.isLoggedIn()){		
		me.cut(node, trackingComponent);
		me.paste(node, trackingComponent);
		me.rename(node, trackingComponent);		
		me.del(node, trackingComponent);	
		me.toRoot(node, trackingComponent);
	}	
	return me.menu;	
};

WTExplorerMenu.prototype.cut = function(node, trackingComponent) {
	var me = this;
	me.menu.cut = {
		label: "Cut",
		action: function (obj) {
			WTTracker.track({
    			component: trackingComponent,
    			actiontype: WTTracker.t.cut,
    			action: 'cut task',
    			taskId: node.id
    		});
			me.source = node.id;
		},
		icon: me.baseurl+'/extensions/WorkflowTasks/images/task/cut.png',
	};
};

WTExplorerMenu.prototype.paste = function(node, trackingComponent) {
	var me = this;
	var target = node.id;
	var source = me.source;
	var disable = !source || source  == target || me.expapi.isChild(target, source);
	var fade = disable ? '_fade' : '';
	me.menu.paste = {
		label: "Paste",
		action: function (obj) {
			WTTracker.track({
    			component: trackingComponent,
    			actiontype: WTTracker.t.move,
    			action: 'paste task',
    			taskId: source,
    			value: target
    		});
			me.expapi.lockAll('Move Task...');			
			var selId = me.expapi.selectedTaskId();			
			me.api.moveTask(me.source, target, selId, function(data){
        		me.expapi.notifyAll(data.wttasks.update);
        	});
			delete me.source;
		},
		icon: me.baseurl+'/extensions/WorkflowTasks/images/task/paste'+fade+'.png',
		_disabled: disable
	};
};

WTExplorerMenu.prototype.rename = function(node, trackingComponent) {
	var me = this;
	me.menu.rename = {
		label: "Rename",
		action: function (obj) {
			$tree = $('#'+obj.reference.context.id);
			var jstree = $tree.jstree(true);
			jstree.rename_node(node , node.data.title);
			jstree.edit(node);
			$tree.on('rename_node.jstree', function(obj, value){
				var newname = value.text;
				var task = me.expapi.taskInfo(node.id);
				if(newname != task.title){
					console.log(node);
					console.log(value);
					var taskId = value.node.id;
					WTTracker.track({
		    			component: trackingComponent,
		    			actiontype: WTTracker.t.rename,
		    			action: 'rename task',
		    			taskId: taskId,
		    			value: newname
		    		});
					me.expapi.lockAll('Rename Task...');			
					var selId = me.expapi.selectedTaskId();			
					me.api.renameTask(taskId, newname, selId, function(data){
						if('forward' in data.wttasks)
		        			document.location.href = me.baseurl+'/index.php/'+data.wttasks.forward;
		        		me.expapi.notifyAll(data.wttasks.update);
		        		if(data.wttasks.exist){		        			
		                	$content = $('<div>Task <b>'+task.title+'</b> can not be renamed to <b>'+newname+'</b>! The new name already exist! </div>');
			            	$content.css('background-image', 'url('+task.icon+')');
			            	$content.css('padding-left', '35px');
		        			new WTDialog({
		        				icon: me.baseurl+'/extensions/WorkflowTasks/images/task/warning.png',
		        				title: 'Already exist',
		        				content: $content,
		        				options: ['Ok']
		        			});
		        		}		        		
		        	});
				}
			});

		},
		icon: me.baseurl+'/extensions/WorkflowTasks/images/task/rename.png'
	};
};

WTExplorerMenu.prototype.toRoot = function(node, trackingComponent) {
	var me = this;
	if(!me.expapi.isRoot(node.id)){
		me.menu.toroot = {
	        label: "To Toplevel",
	        action: function (obj) {
	        	WTTracker.track({
	    			component: trackingComponent,
	    			actiontype: WTTracker.t.move,
	    			action: 'to toplevel',
	    			taskId: node.id
	    		});
	        	me.expapi.lockAll('Move Task to Toplevel...');
	        	var selId = me.expapi.selectedTaskId();
	        	me.api.moveTaskToRoot(node.id, selId, function(data){
	        		me.expapi.notifyAll(data.wttasks.update);
	        	});
	        },
	        icon: me.baseurl+'/extensions/WorkflowTasks/images/task/top.png',
		};
	}
};

WTExplorerMenu.prototype.del = function(node, trackingComponent) {
	var me = this;
	var icon = me.baseurl+'/extensions/WorkflowTasks/images/task/del.png';
	me.menu.del = {
		label: "Delete",
        action: function (obj) {
        	var task = me.expapi.taskInfo(node.id);
        	$content = $('<div>Delete task <b>'+task.title+'</b> with all '+task.subcount+' Subtasks?</div>');
        	$content.css('background-image', 'url('+task.icon+')');
        	$content.css('padding-left', '35px');
        	$content.css('min-height', '35px');
			new WTDialog({
				icon: icon,
				title: 'Delete Task',
				content: $content,
				options: ['Delete', 'Cancel'],
				callback: {
					func: me.delConfirm,
					data: {taskId:node.id,trackingComponent:trackingComponent},
					obj: me
				}
			});
        },
        icon: icon,
	};
};

WTExplorerMenu.prototype.delConfirm = function(option, data) {
	var taskId = data.taskId;
	var trackingComponent = data.trackingComponent;
	var me = this;
	if(option == 'Delete'){
		me.expapi.lockAll('Delete Task...');
		var selId = me.expapi.selectedTaskId();
	  	me.api.deleteTask(taskId, selId, function(data) {
	  		var d = data.wttasks;
			if(d.forward == true){
				var id = d.update.explorer.length>0 ? d.update.explorer[0].id : 'Main_Page';
				document.location.href = me.baseurl+'/index.php/'+id;
	  		}else
				me.expapi.notifyAll(d.update);	        		
			});
		WTTracker.track({
			component: trackingComponent,
			actiontype: WTTracker.t.del,
			action: 'del task',
			taskId: taskId
		});
	}
};
