var WTAPI = function(title, apiuri, editToken) {
	this.title = title;
	this.apiuri = apiuri;
	this.editToken = editToken ? editToken : "";
};

WTAPI.prototype.setType = function( task, type, callbackfunction ) {
	this.setTaskMetaFact( task, "Type", type, callbackfunction );
};

WTAPI.prototype.setStart = function( task, start, callbackfunction ) {	
	this.setTaskMetaFact( task, "StartDate", start, callbackfunction );
};

WTAPI.prototype.setTarget = function( task, target, callbackfunction ) {
	this.setTaskMetaFact( task, "TargetDate", target, callbackfunction );
};

WTAPI.prototype.setProgress = function( task, progress, callbackfunction ) {
	this.setTaskMetaFact( task, "Progress", progress, callbackfunction );
};

WTAPI.prototype.setOwner = function( task, owner, callbackfunction ) {
	this.setTaskMetaFact( task, "Owner", owner, callbackfunction );
};

WTAPI.prototype.addExpertise = function( task, expertise, callbackfunction ) {
	this.addTaskMetaFact( task, 'Expertise', expertise, callbackfunction );
};

WTAPI.prototype.removeExpertise = function( task, expertise, callbackfunction ) {
	this.delTaskMetaFact( task, 'Expertise', expertise, callbackfunction );
};

WTAPI.prototype.addParticipant = function( task, participant, callbackfunction ) {
	this.addTaskMetaFact( task, 'Participants', participant, callbackfunction );
};

WTAPI.prototype.removeParticipant = function( task, participant, callbackfunction ) {
	this.delTaskMetaFact( task, 'Participants', participant, callbackfunction );
};

WTAPI.prototype.removeSubTask = function( task, subtask, callbackfunction ) {
	this.delTaskMetaFact( task, 'SubTask', subtask, callbackfunction );
};

WTAPI.prototype.setTaskMetaFact = function( title, property, value, callbackfunction ) {
	$.post(this.apiuri, {
		"action"   	: "wttasks",
		"operation"	: "setmeta",
		"title"	   	: title,
		"property" 	: property,		
		"value" 	: value,	
		"format"   	: "json"
	}, callbackfunction, "json");
};

WTAPI.prototype.addTaskMetaFact = function( title, property, value, callbackfunction ) {
	$.post(this.apiuri, {
		"action"   	: "wttasks",
		"operation"	: "addmeta",
		"title"	   	: title,
		"property" 	: property,		
		"value" 	: value,	
		"format"   	: "json"
	}, callbackfunction, "json");
};

WTAPI.prototype.delTaskMetaFact = function( title, property, value, callbackfunction ) {
	$.post(this.apiuri, {
		"action"   	: "wttasks",
		"operation"	: "delmeta",
		"title"	   	: title,
		"property" 	: property,		
		"value" 	: value,	
		"format"   	: "json"
	}, callbackfunction, "json");
};

WTAPI.prototype.addTask = function( title, subtitle, isRoot, owner, expertise, callbackfunction ) {
	$.post(this.apiuri, {
		"action"   : "wttasks",
		"operation": "add",
		"title"	   : title,
		"subtitle" : subtitle,	
		"isroot"   : isRoot,
		"owner"	   : owner,
		"expertise": expertise,
		"format"   : "json"
	}, callbackfunction, "json");
};

WTAPI.prototype.moveTask = function( souceTask, targetTask, selectedTaskId, callbackfunction ) {
	$.post(this.apiuri, {
		"action"   	: "wttasks",
		"operation"	: "move",
		"title"		: selectedTaskId,
		"source"   	: souceTask,
		"target"   	: targetTask,		
		"format"   	: "json"
	}, callbackfunction, "json");
};

WTAPI.prototype.moveTaskToRoot = function( souceTask, selectedTaskId, callbackfunction ) {
	$.post(this.apiuri, {
		"action"   	: "wttasks",
		"operation"	: "toroot",
		"title"		: selectedTaskId,
		"source"   	: souceTask,		
		"format"   	: "json"
	}, callbackfunction, "json");
};

WTAPI.prototype.renameTask = function( oldtitle, newtitle, selectedTaskId, callbackfunction ) {
	$.post(this.apiuri, {
		"action"   	: "wttasks",
		"operation"	: "rename",
		"title"		: selectedTaskId,
		"oldtitle"  : oldtitle,		
		"newtitle"  : newtitle,		
		"format"   	: "json"
	}, callbackfunction, "json");
};

WTAPI.prototype.deleteTask = function( task, selectedTaskId, callbackfunction ) {
	$.post(this.apiuri, {
		"action"   	: "wttasks",
		"operation"	: "del",
		"title"		: selectedTaskId,
		"delId"    	: task,	
		"format"   	: "json"
	}, callbackfunction, "json");
};

WTAPI.prototype.updateExplorer = function( title, callbackfunction, callbackObj) {
	$.post(this.apiuri, {
		"action"   : "wttasks",
		"operation": "getExplorer",
		"title"    : title,
		"format"   : "json"
	}, function(data){
		callbackfunction.apply(callbackObj, [data.wttasks.result]);
	}, "json");
};

WTAPI.prototype.addSimpleSubTask = function( task, subtask, callbackfunction ) {
	this.addFact( task, 'SubTask', subtask, callbackfunction );
};

WTAPI.prototype.removeSimpleSubTask = function( task, subtask, callbackfunction ) {
	this.removeFact( task, 'SubTask', subtask, callbackfunction );
};

WTAPI.prototype.addAnswer = function( task, answer, callbackfunction ) {
	this.addFact( task, 'Answer', answer, callbackfunction );
};

WTAPI.prototype.removeAnswer = function( task, answer, callbackfunction ) {
	this.removeFact( task, 'Answer', answer, callbackfunction );
};

WTAPI.prototype.addWorkflow = function( title, url, callbackfunction ) {
	this.addFact( title, 'Workflow', url, callbackfunction );
};

WTAPI.prototype.removeWorkflow = function( title, url, callbackfunction ) {
	this.removeFact( title, 'Workflow', url, callbackfunction );
};

WTAPI.prototype.addExecutedWorkflow = function( title, url, callbackfunction ) {
	this.addFact( title, 'ExecutedWorkflow', url, callbackfunction );
};

WTAPI.prototype.removeExecutedWorkflow = function( title, url, callbackfunction ) {
	this.removeFact( title, 'ExecutedWorkflow', url, callbackfunction );
};

WTAPI.prototype.addDataLink = function( title, url, callbackfunction ) {
	this.addFact( title, 'DataLocation', url, callbackfunction );
};

WTAPI.prototype.removeDataLink = function( title, url, callbackfunction ) {
	this.removeFact( title, 'DataLocation', url, callbackfunction );
};

WTAPI.prototype.addDataWikiLink = function( title, url, callbackfunction ) {
	this.addFact( title, 'DataWikiLocation', url, callbackfunction );
};

WTAPI.prototype.removeDataWikiLink = function( title, url, callbackfunction ) {
	this.removeFact( title, 'DataWikiLocation', url, callbackfunction );
};

WTAPI.prototype.addDataExtractedFrom = function( title, url, callbackfunction ) {
	this.addFact( title, 'DataExtractedFrom', url, callbackfunction );
};

WTAPI.prototype.removeDataExtractedFrom = function( title, url, callbackfunction ) {
	this.removeFact( title, 'DataExtractedFrom', url, callbackfunction );
};

WTAPI.prototype.addDataColumn = function( title, col, newlist, callbackfunction ) {
	var subobjs = [];
	for(var i=0; i<newlist.length; i++) 
		subobjs.push({id:newlist[i], properties: {'Index':i}});
	this.addFactComplex( title, 'Columns', col, subobjs, callbackfunction );
};

WTAPI.prototype.removeDataColumn = function( title, col, newlist, callbackfunction ) {
	var subobjs = [];
	for(var i=0; i<newlist.length; i++) 
		subobjs.push({id:newlist[i], properties: {'Index':i}});
	this.removeFactComplex( title, 'Columns', col, subobjs, callbackfunction );
};

WTAPI.prototype.moveDataColumn = function( title, newlist, callbackfunction ) {
	var subobjs = [];
	for(var i=0; i<newlist.length; i++) 
		subobjs.push({id:newlist[i], properties: {'Index':i}});
	this.updateSubobjects( title, JSON.stringify(subobjs), callbackfunction );
};

WTAPI.prototype.createPageWithCategory = function( title, category, callbackfunction ) {
	$.post(this.apiuri, {
		"action"   : "wtfacts",
		"operation": "newpage",
		"title"    : title,
		"category" : category,
		"format"   : "json",
		"token"    : this.editToken
	}, callbackfunction, "json");
};

WTAPI.prototype.updateSubobjects = function( title, subobjectsjson, callbackfunction ) {
	$.post(this.apiuri, {
		"action"          : "wtfacts",
		"operation"       : "updatesubobjects",
		"title"           : title,
		"subobjectsjson"  : subobjectsjson,
		"format"          : "json",
		"token"           : this.editToken
	}, function(data){
		callbackfunction(data, self);
	}, "json");
};

WTAPI.prototype.addFact = function( subject, predicate, object, callbackfunction ) {
	this.addFactRaw(subject, predicate, object, null, callbackfunction);
};

WTAPI.prototype.removeFact = function( subject, predicate, object, callbackfunction ) {
	this.removeFactRaw(subject, predicate, object, null, callbackfunction);
};

WTAPI.prototype.setFact = function( subject, predicate, object, callbackfunction ) {
	this.setFactRaw(subject, predicate, object, null, callbackfunction);
};

WTAPI.prototype.replaceFact = function( subject, predicate, object, oldobject, callbackfunction ) {
	this.replaceFactRaw(subject, predicate, object, oldobject, null, callbackfunction);
};

WTAPI.prototype.addFactComplex = function( subject, predicate, object, subobjs, callbackfunction ) {
	this.addFactRaw(subject, predicate, object, JSON.stringify(subobjs), callbackfunction);
};

WTAPI.prototype.removeFactComplex = function( subject, predicate, object, subobjs, callbackfunction ) {
	this.removeFactRaw(subject, predicate, object, JSON.stringify(subobjs), callbackfunction);
};

WTAPI.prototype.setFactComplex = function( subject, predicate, object, subobjs, callbackfunction ) {
	this.setFactRaw(subject, predicate, object, JSON.stringify(subobjs), callbackfunction);
};

WTAPI.prototype.replaceFactComplex = function( subject, predicate, object, oldobject, subobjs, callbackfunction ) {
	this.replaceFactRaw(subject, predicate, object, oldobject, JSON.stringify(subobjs), callbackfunction);
};

WTAPI.prototype.addFactRaw = function( subject, predicate, object, subobjectsjson, callbackfunction ) {
	$.post(this.apiuri, {
		"action"          : "wtfacts",
		"operation"       : "add",
		"title"           : subject,
		"property"        : predicate,
		"value"           : object,
		"subobjectsjson"  : subobjectsjson,
		"format"          : "json",
		"token"           : this.editToken
	}, function(data){
		if(data.wtfacts.result == "Error") {
			noty({text: data.wtfacts.text, type:'error', theme:'relax'});
		}
		callbackfunction(data, self);
	}, "json");
};

WTAPI.prototype.removeFactRaw = function( subject, predicate, object, subobjectsjson, callbackfunction ) {
	$.post(this.apiuri, {
		"action"          : "wtfacts",
		"operation"       : "del",
		"title"           : subject,
		"property"        : predicate,
		"value"           : object,
		"subobjectsjson"  : subobjectsjson,
		"format"          : "json",
		"token"    : this.editToken
	}, callbackfunction, "json");
};


WTAPI.prototype.replaceFactRaw = function( subject, predicate, object, oldobject, subobjectsjson, callbackfunction ) {
	$.post(this.apiuri, {
		"action"          : "wtfacts",
		"operation"       : "replace",
		"title"           : subject,
		"property"        : predicate,
		"value"           : object,
		"oldvalue"        : oldobject,
		"subobjectsjson"  : subobjectsjson,
		"format"          : "json",
		"token"    : this.editToken
	}, callbackfunction, "json");
};

WTAPI.prototype.setFactRaw = function( subject, predicate, object, subobjectsjson, callbackfunction ) {
	$.post(this.apiuri, {
		"action"          : "wtfacts",
		"operation"       : "set",
		"title"           : subject,
		"property"        : predicate,
		"value"        	  : object,
		"subobjectsjson"  : subobjectsjson,
		"format"          : "json",
		"token"    : this.editToken
	}, callbackfunction, "json");
};


WTAPI.prototype.getFacts = function( subject, predicate, object, callbackfunction ) {
	$.post(this.apiuri, {
		"action"   : "wtfacts",
		"operation": "show",
		"title"    : subject,
		"format"   : "json"
	}, callbackfunction, "json");
};

WTAPI.prototype.getSuggestions = function( search, type, callbackfunction ) {
	$.post(this.apiuri, {
		"action"   : "wtsuggest",
		"type"     : type,
		"search"   : search,
		"format"   : "json"
	}, callbackfunction, "json");
};

WTAPI.prototype.updateDocu = function(callbackfunction) {
	$.post(this.apiuri, {
		"action"   : "wtadmin",
		"operation": "updatedocu",
		"format"   : "json"
	}, callbackfunction, "json");
};

WTAPI.prototype.addTraining = function(fullname, username, callbackfunction) {
	$.post(this.apiuri, {
		"action"   : "wtadmin",
		"operation": "addtraining",
		"fullname" : fullname,
		"username" : username,
		"format"   : "json"
	}, callbackfunction, "json");
};

WTAPI.prototype.getAllCategories = function(callbackfunction) {
	var me = this;
	$.getJSON(this.apiuri, {
		"action"   : "query",
		"list"     : "allcategories",
		"format"   : "json"
	}, function(result) {
		var categories = new Array();
		for(var i=0; i<result.query.allcategories.length; i++) 
			categories.push(result.query.allcategories[i]["*"]);

		callbackfunction(categories);
	});
};

WTAPI.prototype.setToken = function() {
	var me = this;
	$.getJSON(this.apiuri, {
		"action"   : "query",
		"prop"     : "info",
		"intoken"  : "edit",
		"titles"   : this.title,
		"format"   : "json"
	}, function(result) {
		for (i in result.query.pages)
			me.editToken =  result.query.pages[i].edittoken;
	});
};

