var WTExplorerParser = function() {	
	this.tasks = {};
	this.rootTasks = {}; 			// [id] = id
	this.hasParent = {}; 			// [id] = parentId
	this.hasChild = {};  			// [id] = [childId1, childId2, ...]
	this.expFilters = {};   		// [expertis] = count, expertise filters
	this.ownerFilters = {}; 		// [owner] = count, owner filters
	this.participantFilters = {}; 	// [participant] = count, owner filters
	this.mytaskCount = 0; 
};

WTExplorerParser.prototype.parse = function(data) {
	var me = this;
	me.data = data;
	/** set root tasks */
	$.each(data.explorer, function(k, task) {
		me.rootTasks[task.id] = task.id;
	});
	
	/** set hasParent, hasChild */
	$.each(data.explorer, function(k, task) {
		me.recinit(task, false);
	});
	return me;
};

/**
 * Recursivly initialzes the data.
 * Only done once after object creation.
 */
WTExplorerParser.prototype.recinit = function(task, parentId) {
	var me = this;	
	if(parentId)
		me.hasParent[task.id] = parentId;	
	if(task.children && task.children.length > 0)
		$.each(task.children, function(k, child) {
			if(!me.hasChild[task.id])
				me.hasChild[task.id] = [];			
			me.hasChild[task.id].push(child.id);
			me.recinit(child, task.id);
		});
	
	me.tasks[task.id] = JSON.parse(JSON.stringify(task)); //TODO copy attribtes instrad of stringlify deep copy, children need to be cutted
	delete me.tasks[task.id].children;
	
	$.each(task.facts.expertise, function(key, e) {
		if(me.expFilters[e])
			me.expFilters[e]++;
		else
			me.expFilters[e] = 1;	
	});
	
	var o = task.facts.owner;
	if(me.ownerFilters[o])
		me.ownerFilters[o]++;
	else
		me.ownerFilters[o] = 1;	
	
	$.each(task.facts.participants, function(key, p){
		if(me.participantFilters[p])
			me.participantFilters[p]++;
		else
			me.participantFilters[p] = 1;	
	});	
	
	var user = me.data.user.name;
	if(task.facts.owner == user || task.facts.participants.indexOf(user)!=-1)
		me.mytaskCount++;
};
