var Eval = function() {
	this.u = "unknown";
	this.taskPersonsArr = []; //[p1,p2,p3][p2,p1,p5][p4]
	this.persons = {};
	this.personPairs = {};
	this.tasks = [];
};

Eval.prototype.eval = function() {
	var me = this;
	$.getJSON( "explorer2.json", function( data ) {
		$.each(data.explorer, function(k, task) {
			me.recinit(task, false, 0);
		});		 
//		console.log('--persons per task---------');
//		me.personsPerTask();
//		console.log(JSON.stringify(me.createPersonPairs(['A','B','C','D','E'])));
//		me.createPersonPairs(['A','B','C','D','E']);
//		console.log('--person network---------');
		me.personNetwork();
//		console.log(JSON.stringify(me.personPairs));
		me.personNetworkToCSV();
//		me.nestingFactor();
		me.taskChildrenSplit();
	});		
};

Eval.prototype.recinit = function(task, parentId, level) {
	var me = this;	
	var t = JSON.parse(JSON.stringify(task)); 
//	delete t.children;
	t.level = level;
	me.tasks.push(t);
	
	if(task.children && task.children.length > 0)
		$.each(task.children, function(k, child) {
			me.recinit(child, task.id, level+1);
		});
};

Eval.prototype.personsPerTask = function() {
	var me = this;
	var pptask = {}; //[nr of persns] = #tasks
	var nrOfTasks = me.tasks.length;
	$.each(me.tasks, function(k, t) {
		nrPersons = me.taskPersons(t).length;
		if(!pptask[nrPersons])
			pptask[nrPersons] = {count:0};
		pptask[nrPersons].count++;
		pptask[nrPersons].percentage = pptask[nrPersons].count/nrOfTasks;
	});
	
	console.log(JSON.stringify(pptask));
};


Eval.prototype.taskPersons = function(task) {
	var me = this;
	var ps = [];
	
	if(task.facts.owner != me.u)
		ps.push(task.facts.owner);	
	
	$.each(task.facts.participants, function(k, p) {
		if(p != me.u)
			ps.push(p);
	});	
	return ps;
};
//
Eval.prototype.createPersonPairs = function(persons) {
	var me = this;
	persons.sort();
	if(persons.length>1){
		for(i1 = 0; i1 < persons.length-1; i1++){ 
			p1 = persons[i1];
			for(i2 = i1+1; i2 < persons.length; i2++){ 
				p2 = persons[i2];
				pairId = p1+'_'+p2;
				if(!me.personPairs.hasOwnProperty(pairId))
					me.personPairs[pairId] = {persons:[p1,p2], count:0};
				me.personPairs[pairId].count++;
			}
		}
	}
//	console.log(JSON.stringify(me.personPairs));
//	console.log(Object.keys(me.personPairs).length);
}

Eval.prototype.personNetwork = function() {
	var me = this;
	var pairs = [];
	$.each(me.tasks, function(k, t) {
		var persons = me.taskPersons(t);
//		console.log(JSON.stringify(persons));
		persons.sort();
		if(persons.length>1){
			for(i1 = 0; i1 < persons.length-1; i1++){ 
				p1 = persons[i1];
				for(i2 = i1+1; i2 < persons.length; i2++){ 
					p2 = persons[i2];
					if(p1 != p2){
						pairId = p1+'_'+p2;
//						console.log(pairId);
						if(!me.personPairs.hasOwnProperty(pairId))
							me.personPairs[pairId] = {persons:[p1,p2], count:0};
						me.personPairs[pairId].count++;
					}
				}
			}
		}
	});
};

Eval.prototype.personNetworkToCSV = function() {
	var me = this;
	
//	var csv = 'Node, Id, Label\n';
//	$.each(me.personPairs, function(k, edge) {
//		console.log(edge);
//		csv += '"'+edge.persons[0]+'", "'+edge.persons[1]+'", '+edge.count+'\n';
//	});
//	var encodedUri = encodeURI(csv);
//	var link = document.createElement("a");
//	link.setAttribute("href", 'data:text/csv;charset=utf-8,' +encodedUri);
//	link.setAttribute("download", "personNetworkEdges.csv");
//	link.click(); 	
	
	
	var csv = 'Source,Target,Type,Weight\n';
	$.each(me.personPairs, function(k, edge) {
//		console.log(edge);
		csv += edge.persons[0]+','+edge.persons[1]+',Undirected,'+edge.count+'\n';
	});
	var encodedUri = encodeURI(csv);
	var link = document.createElement("a");
	link.setAttribute("href", 'data:text/csv;charset=utf-8,' +encodedUri);
	link.setAttribute("download", "personNetworkEdges.csv");
	link.click(); 	
	
//	console.log(csv);
}

Eval.prototype.nestingFactor = function() {
	//ancestor
	var me = this;
	console.log('--nesting factor--');
	var tasksPerLevel = [];
	var levelsum = 0;
	$.each(me.tasks, function(k, task) {
		levelsum+=task.level;
		if(tasksPerLevel[task.level])
			tasksPerLevel[task.level]++;
		else
			tasksPerLevel[task.level] = 1;		
	});
	console.log('Level: Number of Tasks');
	$.each(tasksPerLevel, function(level, nrOfTasks) {
		console.log(level+': '+nrOfTasks);
	});
	var avgLevel = levelsum/me.tasks.length;
	console.log('AVG Level: '+avgLevel);
	
	var shelp = 0;
	$.each(tasksPerLevel, function(level, nrOfTasks) {
		shelp += (level-avgLevel)*(level-avgLevel);
	});
	var ssquare = shelp/me.tasks.length
	console.log('s²: '+ssquare);
	console.log('s: '+Math.sqrt(ssquare));	
};

Eval.prototype.taskChildren = function() {
	var me = this;
	console.log('\n--task children-');
	var childrenPerTask = [];
	var childrensum = 0;
	$.each(me.tasks, function(k, task) {		
		var nrOfChilds = 0;
		if(task.children)
			nrOfChilds = task.children.length;
		childrensum+=nrOfChilds;
		if(childrenPerTask[nrOfChilds])
			childrenPerTask[nrOfChilds]++;
		else
			childrenPerTask[nrOfChilds] = 1;		
	});
//	console.log(JSON.stringify(childrenPerTask));
	console.log('Children: Number of Tasks');
	$.each(childrenPerTask, function(children, nrOfTasks) {
		console.log(children+': '+nrOfTasks);
	});
	var avgChildren= childrensum/me.tasks.length;
	console.log('AVG Children: '+avgChildren);
	
	var shelp = 0;
	$.each(childrenPerTask, function(children, nrOfTasks) {
		shelp += (children-avgChildren)*(children-avgChildren);
	});
	var ssquare = shelp/me.tasks.length
	console.log('s²: '+ssquare);
	console.log('s: '+Math.sqrt(ssquare));	
};

Eval.prototype.taskChildrenSplit = function() {
	var me = this;
	me.taskChildrenExtended(true);
	me.taskChildrenExtended(false);
};

Eval.prototype.taskChildrenExtended = function(metalevel) {
	var me = this;
	console.log('\n--task children-metalevel:'+metalevel);
	var childrenPerTask = [];
	var childrensum = 0;
	var childcount = 0;
	$.each(me.tasks, function(k, task) {
		var countContidtion;
		if(metalevel)
			countContidtion = task.facts.missingparam;
		else
			countContidtion = !task.facts.missingparam;
		if(countContidtion){
			var nrOfChilds = 0;
			if(task.children)
				nrOfChilds = task.children.length;
			childrensum+=nrOfChilds;		
			if(childrenPerTask[nrOfChilds])
				childrenPerTask[nrOfChilds]++;
			else
				childrenPerTask[nrOfChilds] = 1;
			childcount++;
		}
	});
//	console.log(JSON.stringify(childrenPerTask));
	console.log('Children: Number of Tasks');
	$.each(childrenPerTask, function(children, nrOfTasks) {
		console.log(children+': '+nrOfTasks);
	});
	var avgChildren= childrensum/childcount;
	console.log('AVG Children: '+avgChildren);
	
	var shelp = 0;
	$.each(childrenPerTask, function(children, nrOfTasks) {
		shelp += (children-avgChildren)*(children-avgChildren);
	});
	var ssquare = shelp/childcount;
	console.log('s²: '+ssquare);
	console.log('s: '+Math.sqrt(ssquare));	
};

