
var WTExplorerAPI = function(data, baseurl, api){	
	this.baseurl = baseurl;
	this.api = api;
	this.subscribers = [];	
	this.initData(data);	
};

WTExplorerAPI.prototype.initData = function(data) {
	var p = new WTExplorerParser().parse(data);
	this.tasks = p.tasks;							// [id] = task details
	this.rootTasks =  p.rootTasks; 					// [id] = id
	this.hasParent =  p.hasParent; 					// [id] = parentId
	this.hasChild = p.hasChild;  					// [id] = [childId1, childId2, ...]
	this.expFilters = p.expFilters;   				// [expertis] = count, expertise filters
	this.ownerFilters = p.ownerFilters; 			// [owner] = count, owner filters
	this.participantFilters = p.participantFilters; // [owner] = count, owner filters
	this.mytaskCount = p.mytaskCount; 				// Worklist count
	this.data = p.data;
};

WTExplorerAPI.prototype.allExpertiseFilters = function() {
	return this.expFilters;
};

WTExplorerAPI.prototype.myTaskCount = function() {
	return this.mytaskCount;
};

WTExplorerAPI.prototype.isLoggedIn = function() {
	return this.data.user.isLoggedIn;
};

/** full name of the user e.g. "Max Mustermann" **/
WTExplorerAPI.prototype.username = function() {
	return this.data.user.name;
};

/** user id  e.g. "Admin" **/
WTExplorerAPI.prototype.userid = function() {
	return this.data.user.id;
};

WTExplorerAPI.prototype.selectedTaskId = function() {
	return this.data.selectedId;
};

WTExplorerAPI.prototype.taskExist = function(taskId) {
	var me = this;
	taskId = taskId.toLowerCase();
	if(!me.existingTasks){
		me.existingTasks = Object.keys(me.tasks);
		$.each(me.existingTasks , function(i, taskId) {
			me.existingTasks[i] = taskId.toLowerCase();
		});
	}
	return me.existingTasks.indexOf(taskId)!=-1;
};

WTExplorerAPI.prototype.isRoot = function (taskId){
	return taskId in this.rootTasks;
};

WTExplorerAPI.prototype.isChild = function (childId, parentId){
	var me = this;
	var exist = false;
	if(parentId && parentId in me.hasChild){
		$.each(me.hasChild[parentId], function(k, cId){
			if(cId == childId)
				exist = true;
			exist = exist || me.isChild(childId, cId);
		});
	}
	return exist;
};

WTExplorerAPI.prototype.taskInfo = function (taskId){
	var me = this;
	var task = me.tasks[taskId];
	return {
		title: task.title,
		subcount: me.countChilds(taskId),
		icon: me.generateIconPath(task.facts, 'large', false)
	}
};

WTExplorerAPI.prototype.countChilds = function (taskId){
	var me = this;
	var count = 0;
	if(taskId in me.hasChild)
		$.each(me.hasChild[taskId], function(k, childId){
			count += 1 + me.countChilds(childId);
		});
	return count;
};

/**
 * (Public) Applay filter applies the paramter filter on the base data.
 * @param f filter
 */
WTExplorerAPI.prototype.applayFilter = function(f, view) {
	var me = this;	
	var m = me.findMatchingTasks(f);	
	if(view == 'hierachie'){		
		var p = me.findParentTasks(m);
		var c = me.findChildTasks(m, p);	
		return me.generateExplorer(me.rootTasks, p, m, c, me.expand(f));
	}else{
		return me.generateList(m);
	}
};

/** (Private) Find all direct matching tasks */
WTExplorerAPI.prototype.findMatchingTasks = function(f){
	var me = this;	
	/** determin nr of filters for matching result */
	var nrOfFilters = Object.keys(f).length;
	var matchingTasks = {}; 
	var uxtoday = new Date().getTime()/1000;
	var u = 'unknown';
	$.each(me.tasks, function(id, t) {
		var nrOfMatches = 0;
		if('text' in f && t.title.toLowerCase().search(f.text.toLowerCase())!=-1)
			nrOfMatches++;		
		if('expertise' in f && t.facts.expertise.indexOf(f.expertise.toLowerCase())!=-1)
			nrOfMatches++;
		if('owner' in f && t.facts.owner == f.owner)
			nrOfMatches++;	
		if('participant' in f && t.facts.participants.indexOf(f.participant)!=-1)
			nrOfMatches++;	
		if('mytasks' in f && (t.facts.participants.indexOf(f.mytasks)!=-1 || t.facts.owner == f.mytasks))
			nrOfMatches++;	
		if('overdue' in f && t.facts.overdue == f.overdue)
			nrOfMatches++;	
		if('infuture' in f && (t.facts.start > uxtoday || t.facts.start == u))
			nrOfMatches++;	
		if('incurrent' in f && t.facts.start < uxtoday && uxtoday < t.facts.target)
			nrOfMatches++;	
		if('inpast' in f && t.facts.target < uxtoday && !t.facts.overdue)
			nrOfMatches++;	
		if(nrOfFilters == nrOfMatches)
			matchingTasks[id]=true;
	});
	return matchingTasks;
};

/** (Private) Find all parent tasks of machtching tasks*/
WTExplorerAPI.prototype.findParentTasks = function(matchingTasks){
	var me = this;
	var parentTasks = {};
	$.each(matchingTasks, function(mtask) {
		var pt = me.hasParent[mtask];
		while(pt && !parentTasks[pt] && !matchingTasks[pt]){		
			parentTasks[pt] = true;
			pt = me.hasParent[pt];			
		}
	});
	return parentTasks;
}

/** (Private) Find all child tasks of matching tasks */
WTExplorerAPI.prototype.findChildTasks = function(matchingTasks, parentTasks){
	var me = this;
	me.childTasks = {};
	$.each(matchingTasks, function(mtask) {
		me.findChildTaskHelper(mtask, parentTasks, matchingTasks);			
	});
	return me.childTasks;
}

/** (Private) Find all child task helper */
WTExplorerAPI.prototype.findChildTaskHelper = function(task, parentTasks, matchingTasks){
	var me = this;
	var childs = me.hasChild[task];
	if(childs){		
		$.each(childs, function(k, c) {
			if(!parentTasks[c] && !matchingTasks[c]  && !me.childTasks[c]){		
				me.childTasks[c] = true;	
				me.findChildTaskHelper(c, parentTasks, matchingTasks);					
			}
		});
	}
};

/** (Private) Determins if explorer view is expaned by default */
WTExplorerAPI.prototype.expand = function(f){
	var nrOfFilters = Object.keys(f).length;
	if('owner' in f) 
		nrOfFilters--;
	if('expertise' in f) 
		nrOfFilters--;
	return nrOfFilters>0;
};

/** (Private) List view data generation */
WTExplorerAPI.prototype.generateList = function(matchingTasks){
	var me = this;
	var list=[];
	$.each(me.sortTasks(Object.keys(matchingTasks),{start:true}), function(k,mtask) {
		var task = me.tasks[mtask];
		delete task.children;
		task.icon = me.generateIconPath(task.facts, 'medium', false);
		list.push(task);
	});
	return list;
}

/** (Private) Hirachical view data generation */
WTExplorerAPI.prototype.generateExplorer = function(taskIds, parentTasks, matchingTasks, childTasks, expand) {
	var me = this;
	var taskTree = [];
	$.each(me.sortTasks(taskIds,{start:true}), function(k, taskId) {
		if(parentTasks[taskId] != undefined || matchingTasks[taskId] != undefined || childTasks[taskId] != undefined ){
			var task = me.tasks[taskId];	
			if(me.hasChild[taskId] && me.hasChild[taskId].length>0)
				task.children = me.generateExplorer(me.hasChild[taskId], parentTasks, matchingTasks, childTasks, expand);			
			task.state = {opened: expand};
			if(matchingTasks[taskId]){			
				var c = '';
				//if(!task.exist) c='new';
				task.text = '<a class="'+c+'">'+task.title+'</a>';
				task.icon = me.generateIconPath(task.facts, 'medium', false);				
			}else{
				task.text = '<span style="color:#cccccc;">'+task.title+'</span>';
				task.icon = me.generateIconPath(task.facts, 'medium', true);
				if(!parentTasks[taskId])
					task.state = {opened: false};
			}
			task.data = {title:task.title};
			taskTree.push(task);
		}
	});
	return taskTree;
};	

WTExplorerAPI.prototype.sortTasks = function(taskIds, sortBy) {
	var me = this;
	var u = 'unknown';
	var tasks = [];	
	$.each(taskIds, function(k, taskId){
		tasks.push(me.tasks[taskId]);
	});
	if(sortBy.start){
		tasks.sort(function(a,b){
			if(a.facts.start == u && b.facts.start != u)
				return 1;
			if(a.facts.start != u && b.facts.start == u)
				return -1;
			if(a.facts.start == b.facts.start)
				return 0;
			if(a.facts.start > b.facts.start)
				return 1;
			if(a.facts.start < b.facts.start)
				return -1;
			return 0;
		});
	}else if(sortBy.target){
		tasks.sort(function(a,b){
			if(a.facts.target == u && b.facts.target != u)
				return 1;
			if(a.facts.target != u && b.facts.target == u)
				return -1;
			if(a.facts.target == b.facts.target)
				return 0;
			if(a.facts.target > b.facts.target)
				return 1;
			if(a.facts.target < b.facts.target)
				return -1;
			return 0;
		});
		tasks = tasks.reverse();
	}else
		throw{name: "SortTask kriteria not defined!"}; 
	taskIds = [];
	$.each(tasks, function(k, task){
		taskIds.push(task.id);
	});
	return taskIds;
};

/** (Public) Task icon path generation */
WTExplorerAPI.prototype.generateIconPath = function(facts, size, fade){
	var path = this.baseurl+'/extensions/WorkflowTasks/includes/core/api/WTIconAPI.php?icon=task';
	path += '&meta='+facts.missingparam;
	path += '&size='+size;
	path += '&fade='+fade;
	path += '&warning='+(Object.keys(facts.warnings).length>0);
	path += '&childwarning='+facts.childwarning;
	if(facts.missingparam){
		path += '&progress='+facts.metaprogress;
	}else{
		path += '&progress='+facts.progress;		
		path += '&type='+facts.type;	
		path += '&overdue='+facts.overdue;
		path += '&childoverdue='+facts.childoverdue;
	}
	return path;
};

/** (Public) Task new icon path generation */
WTExplorerAPI.prototype.generateNewIconPath = function(){
	return this.generateIconPath({missingparam:true}, 'medium', false);
};

/** (Public) Task context(selected page) view generation */
WTExplorerAPI.prototype.context = function(){
	var me = this;
	var sId = me.data.selectedId;
	var ctx = {};		
	if(me.hasParent[sId]){
		ctx.parent = me.tasks[me.hasParent[sId]];
		ctx.parent.icon = me.generateIconPath(ctx.parent.facts, 'small', false);
		delete ctx.parent.children;
	}
	ctx.selected = me.tasks[sId];
	ctx.selected.icon = me.generateIconPath(ctx.selected.facts, 'large', false);
	delete ctx.selected.children;
	ctx.explorer = me.generateSelectedExplorer(me.hasChild[sId]);
	return ctx;
};

/** (Public) Task meta(selected page) view generation */
WTExplorerAPI.prototype.metadata = function(){
	var me = this;
	var taskId = me.data.selectedId;
	var task = me.tasks[taskId];
	var facts = task.facts;
	var u = 'unknown';
	var meta = {
		type:{
			list: facts.expected.type
		},
		progress:{},			
		start:{},
		target:{},
		owner:{},	
		participants:{},
		expertise:{}				
	};
	$.each(meta, function(k){
		if(k=='expertise' || k=='participants'){
			var exist = !(facts[k].length == 1 && facts[k][0] == u);
			meta[k].exist = exist 
			meta[k].value = exist ? facts[k] : [];			
		}else{
			var exist = facts[k] != u;
			meta[k].exist = exist 
			meta[k].value = exist ? facts[k] : '';					
		}
		meta[k].warning = {};	
		if(k in facts.warnings){
			meta[k].warning.exist = true;
			meta[k].warning.expected = facts.warnings[k];
		}else
			meta[k].warning.exist = false;
	});
	return meta;
};

//WTExplorerAPI.prototype.logDate = function(u){
//	console.log(moment(u, 'X').format('YYYY-MM-DD'));
//};

WTExplorerAPI.prototype.calcStartConstraint = function(target, exist){
	var sId = this.selectedTaskId();
	var s = this.tasks[sId].facts.expected.start;
	if(!exist)
		return {
			lower:s.lower,
			upper:s.upper
		}
	return {
		lower: Math.min(s.lower, target),
		upper: Math.min(s.upper, target),
	};
};

WTExplorerAPI.prototype.calcTargetConstraint = function(start, exist){
	var sId = this.selectedTaskId();
	var t = this.tasks[sId].facts.expected.target;
	if(!exist)
		return {
			lower:t.lower,
			upper:t.upper
		}
	return {
		lower: Math.max(t.lower, start),
		upper: Math.max(t.upper, start),
	};
};

// More restrictive constraints consider also childs but can lead to deadlock with move operations
//WTExplorerAPI.prototype.calcStartConstraint = function(target, exist){
//	var c = this.calcDateConstraints();	
//	if(!exist)
//		return c.start;
//	return {
//		lower: Math.min(c.start.lower, target),
//		upper: Math.min(c.start.upper, target),
//	};
//};
//
//WTExplorerAPI.prototype.calcTargetConstraint = function(start, exist){
//	var c = this.calcDateConstraints();	
//	if(!exist)
//		return c.target;
//	return {
//		lower: Math.max(c.target.lower, start),
//		upper: Math.max(c.target.upper, start),
//	};
//};

WTExplorerAPI.prototype.calcDateConstraints = function(){
	var me = this;
	var task = me.tasks[me.selectedTaskId()];
	var startLower = me.calcStartLowerLimit(task);
	var startUpper = me.calcStartUpperLimit(task);
	var targetLower= me.calcTargetLowerLimit(task);
	var targetUpper = me.calcTargetUpperLimit(task);
	startUpper = Math.min(startUpper, targetUpper);
	targetLower = Math.max(targetLower, startLower);	
	return {
		start:{
			lower: startLower,
			upper: startUpper
		},
		target:{
			lower: targetLower,
			upper: targetUpper
		}
	};
};

WTExplorerAPI.prototype.calcStartLowerLimit = function(task){
	var me = this;
	var start = new Date("1971/01/01").getTime()/1000;
	if(task.id in me.hasParent){
		var parent = me.tasks[me.hasParent[task.id]];
		if(parent.facts.start != 'unknown')
			start = Math.max(start, parent.facts.start);
		start = Math.max(start, me.calcStartLowerLimit(parent));
	}
	return start;
};

WTExplorerAPI.prototype.calcStartUpperLimit = function(task){
	var me = this;
	var start = new Date("2037/01/01").getTime()/1000;		
	if(task && task.id in me.hasChild){
		$.each(me.hasChild[task.id], function(k, childId){
			var child = me.tasks[childId];
			if(child && child.facts.start != 'unknown')
				start = Math.min(start, child.facts.start);
			start = Math.min(start, me.calcStartUpperLimit(child));
		});		
	}
	return start;
};

WTExplorerAPI.prototype.calcTargetUpperLimit = function(task){
	var me = this;
	var target = new Date("2037/01/01").getTime()/1000;
	if(task.id in me.hasParent){
		var parent = me.tasks[me.hasParent[task.id]];
		if(parent.facts.target != 'unknown')
			target = parent.facts.target;
		target = Math.min(target, me.calcTargetUpperLimit(parent));
	}
	return target;
};

WTExplorerAPI.prototype.calcTargetLowerLimit = function(task){
	var me = this;
	var target = new Date("1971/01/01").getTime()/1000;	
	if(task && task.id in me.hasChild){
		$.each(me.hasChild[task.id], function(k, childId){
			var child = me.tasks[childId];
			if(child && child.facts.target != 'unknown')
				target = Math.max(target, child.facts.target);
			target = Math.max(target, me.calcTargetLowerLimit(child));
		});		
	}
	return target;
};


/** Task meta person suggestion*/
WTExplorerAPI.prototype.personSuggestion = function(){
	var me = this;
	var persons = {};
	$.each(me.data.user.persons, function(k,p){
		persons[p] = true;
	});
	$.each(me.ownerFilters, function(p){
		if(p != 'unknown')
			persons[p] = true;
	});
	$.each(me.participantFilters, function(p){
		if(p != 'unknown')
			persons[p] = true;
	});
	var suggestions = [];
	$.each(persons, function(p, count){
		suggestions.push({value: p, data: p});
	});
	return suggestions;
};

WTExplorerAPI.prototype.personExist = function(person){
	return this.data.user.persons.indexOf(person)!=-1
};

/** Task meta owner suggestion*/
WTExplorerAPI.prototype.expertiseSuggestion = function(){
	var me = this;
	var expertise = {};
	$.each(me.data.user.expertise, function(k,e){
		expertise[e] = 0;
	});
	$.each(me.expFilters, function(e, count){
		if(e != 'unknown')
			expertise[e] = count;
	});
	var suggestions = [];
	$.each(expertise, function(e, count){
		suggestions.push({value: e, data: e});
	});
	return suggestions;
};

/** Hierachical view generation for selected part */
WTExplorerAPI.prototype.generateSelectedExplorer = function(taskIds) {
	if(!taskIds) return [];
	var me = this;
	var taskTree = [];
	$.each(me.sortTasks(taskIds,{start:true}), function(k, taskId) {			
		var task = me.tasks[taskId];	
		if(me.hasChild[taskId] && me.hasChild[taskId].length>0)
			task.children = me.generateSelectedExplorer(me.hasChild[taskId]);			
		task.icon = me.generateIconPath(task.facts, 'medium', false);
		taskTree.push(task);
	});
	return taskTree;
};

/** Alert list for logged in users */
WTExplorerAPI.prototype.alertList = function() {
	var me = this;	
	if(me.isLoggedIn()){
		var filter = {owner:me.username(), overdue:true};
		var matchingTasks = me.findMatchingTasks(filter);
		var list = [];
		$.each(matchingTasks, function(mtask) {	
			var t = me.tasks[mtask];
			t.icon = me.generateIconPath(t.facts, 'large', false);
			list.push(t);
		});
		return list;
	}
	return [];
}

/** Data for person page */
WTExplorerAPI.prototype.personsTasks = function(username) {
	var me = this;
	username = username.replace('_',' ');
	return {
		infuture: me.personsTasksData(username, {infuture:true}, {infuture:true}, {target:true}),
		incurrent: me.personsTasksData(username, {incurrent:true}, {incurrent:true}, {start:true}),
		inpast: me.personsTasksData(username, {inpast:true}, {inpast:true}, {target:true}),
		overdue: me.personsTasksData(username, {overdue:true}, {overdue:true}, {target:true})
	};
};

WTExplorerAPI.prototype.personsTasksData = function(username, f1, f2, sortBy) {	
	var me = this;
	f1.owner = username;
	f2.participant = username;
	var matchingTasks = me.findMatchingTasks(f1);	
	$.each(me.findMatchingTasks(f2), function(mtask){
		matchingTasks[mtask] = true;
	});
	var list = {};
	$.each(me.sortTasks(Object.keys(matchingTasks),sortBy), function(k,mtask){
		var t = me.tasks[mtask];
		t.icon = me.generateIconPath(t.facts, 'large', false);
		t.isOwner = t.facts.owner == username;
		delete t.children;
		list[t.id] = t;
	});
	return list;
};


WTExplorerAPI.prototype.personExpertise = function(username) {
	var me = this;
	username = username.replace('_', ' ');
	var matchingTasks = me.findMatchingTasks({owner:username});
	var expertise = {};
	$.each(matchingTasks, function(mtask){
		var t = me.tasks[mtask];
		$.each(t.facts.expertise, function(k,e){
			if(e != 'unknown')
				if(e in expertise)
					expertise[e]+=1;
				else
					expertise[e] =1;
		});		
	});
	return expertise;
};

WTExplorerAPI.prototype.subscribe = function(subscriber) {
	this.subscribers.push(subscriber);
};
WTExplorerAPI.prototype.notifyAll = function(data, notnotify) {
	console.log('notify all');
	this.initData(data);
	$.each(this.subscribers, function(k,subscriber){
		if(subscriber != notnotify)
			subscriber.notify();
	});
};
WTExplorerAPI.prototype.enforceUpdate = function() {
	this.api.updateExplorer(this.selectedTaskId(), this.notifyAll, this);
};
WTExplorerAPI.prototype.lockAll = function(message, notlock) {
	$.each(this.subscribers, function(k,subscriber){
		if (subscriber != notlock && typeof(subscriber.lock) === "function") 
			subscriber.lock(message);
	});
};
