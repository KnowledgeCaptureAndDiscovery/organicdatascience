var WTTaskMetaData = function(title, expapi, wtapi, baseurl, $item) {
	this.$item = $item;
	this.title = title;
	this.expapi = expapi;
	this.wtapi = wtapi;
	this.baseurl = baseurl;
	this.expapi.subscribe(this);
};

WTTaskMetaData.prototype.display = function() {
	this.meta = this.expapi.metadata();
	this.$item.empty();
	this.$item.unmask();
	this.generateTable();
};

WTTaskMetaData.prototype.notify = function() {
	this.display();
};

WTTaskMetaData.prototype.generateTable = function() {
	var me = this;
	me.$table =  $('<div class="wt-table"></div>');
	me.$item.append(me.$table);
	$.each(this.meta, function(ptype, property){
		me.appendRow(property, ptype)
	});
	me.appendNotesRow();
	me.updateIcons();
};

WTTaskMetaData.prototype.appendRow = function(property, ptype) {
	var me = this;
	var $row = $('<div class="wt-row"></div>');
	$row.attr('property', ptype);
	me.$table.append($row);
	$row.click(function(){	
		$t = $(this);
		if(!$t.hasClass('edit') && me.expapi.isLoggedIn()){
			me.closeAllEdits();
			$t.addClass('edit');			
			$c = $t.find('.wt-content');
			me.generateEdit(ptype, $c);
		}				
	});	
	me.appendIconCell($row, ptype, property);
	me.appendContentCell($row, ptype);
	return $row;
};

WTTaskMetaData.prototype.appendNotesRow = function() {
	var me = this;
	var notes  = '<div class="notes">';
		notes += '  <span><b>Legend: </b></span>'
	    notes += '	<span style="font-weight:bolder;">M</span> Mandatory |';
	    notes += '  <span><b>States: </b></span>'
		notes += '	<span style="color:#cccccc; vertical-align: -1px; font-size:17px;">&#9632;</span> Not defined, ';
		notes += '	<span style="color:#b7db7a; vertical-align: -1px; font-size:17px;">&#9632;</span> Valid, ';
		notes += '	<span style="color:#fed800; vertical-align: -1px; font-size:17px;">&#9632;</span> Inconsistent with parent';
		notes += '</div>';
	me.$item.append($(notes));
};

WTTaskMetaData.prototype.closeAllEdits = function() {
	var me = this;
	me.$table.find('.wt-row').each(function(k, row){		
		$row = $(row);
		$row.removeClass('edit');
		var ptype = $row.attr('property');
		$row.find('.wt-content').html(me.generateContent(ptype));
	});
};

WTTaskMetaData.prototype.appendIconCell = function($row, ptype, property) {
	$cell = $('<div class="wt-cell wt-icon"></div>');
	$cell.html(this.typeToLabel(ptype));
	if(property.warning.exist){		
		$msg = $('<div id="wt-metadata-warning"></div>');
		$msg.html(this.generateWarnMsg(ptype, property));
		$cell.tooltipster({
			content: $msg,
			position: 'top-left',
			theme: 'tooltipster-custom-warning',
	        interactive: true,
	        offsetY:-3,
	        maxWidth: 250,
	        arrowColor:'#fffac2',
	        speed:0
		});	
	}
	$row.append($cell);
};

WTTaskMetaData.prototype.generateWarnMsg = function(ptype, property) {
	var r = property.warning.expected;
	switch(ptype){
		case('type'):
			var e = r.length>1 ? 's <b>'+r[0]+'</b> or <b>'+r[1]+'</b>' : ' <b>'+r[0]+'</b>';
			return 'Type is inconsistent with parent task type! Expected task type'+e+'!';
		case('start'): 
			var l = moment.unix(r.lower).format("Do MMM YYYY");   
			var u = moment.unix(r.upper).format("Do MMM YYYY");  
			return 'Start date is inconsistent with parent task start date! Expected start between <b>'+l+'</b> and <b>'+u+'</b>!';
		case('target'): 
			var l = moment.unix(r.lower).format("Do MMM YYYY");   
			var u = moment.unix(r.upper).format("Do MMM YYYY");  
			return 'Target date is inconsistent with parent task target date! Expected target between <b>'+l+'</b> and <b>'+u+'</b>!';
	}
};

WTTaskMetaData.prototype.appendContentCell = function($row, ptype) {	
	$cell = $('<div class="wt-cell wt-content"></div>');
	$cell.append(this.generateContent(ptype));	
	$row.append($cell)
};

WTTaskMetaData.prototype.generateEdit = function(ptype, $content) {
	var me = this;
	p = me.meta[ptype];
	$content.html('');
	switch(ptype){
		case('type'):
			$e = $('<select placeholder="Type"></select>');
			var selected =  !p.exist ? 'selected' : '';
			$e.append('<option '+selected+' value="">Select Type</option>');
			for(i=0; i<p.list.length; i++){
				var selected = '';
				if(p.list[i]==p.value)
					selected = 'selected';
				$e.append('<option '+selected+' value="'+p.list[i]+'">'+p.list[i]+'</option>');
			}
			$e.change(function(e){
				var v = $(this).val();	
				me.expapi.lockAll('Set Task Type...');
				me.wtapi.setType(me.title, v, function(data){
	        		me.expapi.notifyAll(data.wttasks.update);
	        	});
				me.setLocal(ptype, v);
				me.closeAllEdits();
				WTTracker.track({
					component: WTTracker.c.metadata,
					subcomponent: WTTracker.s.type,
					actiontype: WTTracker.t.set,
					action: 'set type',
					taskId: me.title,
					value: v
				});	
			});
			$content.append($e);
			break;
		case('owner'):
			$in = $('<input type="text" placeholder="Owner"/>');
			$in.val(p.value);
			$in.autocomplete2({
				lookup: me.expapi.personSuggestion()
			});
			$in.keyup(function(e){
				if(e.keyCode == 13){
					var v = $in.val();
					me.expapi.lockAll('Set Task Owner...', me);
					me.wtapi.setOwner(me.title, v, function(data){
		        		me.expapi.notifyAll(data.wttasks.update, me);
		        	});
					me.setLocal(ptype, v);
					me.closeAllEdits();
					WTTracker.track({
						component: WTTracker.c.metadata,
						subcomponent: WTTracker.s.owner,
						actiontype: WTTracker.t.set,
						action: 'set owner',
						taskId: me.title,
						value: v
					});	
				}
			});
			$content.append($in);
			$in.focus();
			break;
		case('start'):
			$e = $('<input type="text" placeholder="Start" style="width:80px;"/>');	
			$content.append($e);
			initDateVal(p, $e);						
			setDateLimit(me.meta.target, $e, true);
			$e.change(function(){
				var v = $e.val()
				m = moment(v, "YYYY-MM-DD");
				me.setLocal(ptype, m.format('X'));		
				me.expapi.lockAll('Set Task Start...');
				me.wtapi.setStart(me.title, v, function(data){
	        		me.expapi.notifyAll(data.wttasks.update);
	        	});
				me.closeAllEdits();
				WTTracker.track({
					component: WTTracker.c.metadata,
					subcomponent: WTTracker.s.start,
					actiontype: WTTracker.t.set,
					action: 'set start',
					taskId: me.title,
					value: v
				});	
			});
			break;
		case('target'):
			$e = $('<input type="text" placeholder="Target" style="width:80px;"/>');
			$content.append($e);
			initDateVal(p, $e);
			setDateLimit(me.meta.start, $e, false);
			$e.change(function(){
				var v = $e.val()
				m = moment(v, "YYYY-MM-DD");
				me.setLocal(ptype, m.format('X'));	
				me.expapi.lockAll('Set Task Target...');
				me.wtapi.setTarget(me.title, v, function(data){
	        		me.expapi.notifyAll(data.wttasks.update);
	        	});
				me.closeAllEdits();
				WTTracker.track({
					component: WTTracker.c.metadata,
					subcomponent: WTTracker.s.target,
					actiontype: WTTracker.t.set,
					action: 'set target',
					taskId: me.title,
					value: v
				});	
			});
			
			function initDateVal(property, $input){
				var v = property.value;
				if(!property.exist)
					v = new Date().getTime()/1000;
				$e.val(moment.unix(v).format('YYYY-MM-DD'));
				$e.pickadate({format: 'yyyy-mm-dd'});	
			}
			
			function setDateLimit(property, $input, isStart){				
				if(isStart)
					var c = me.expapi.calcStartConstraint(property.value, property.exist);
				else
					var c = me.expapi.calcTargetConstraint(property.value, property.exist);
				setLimit('max', c.upper, $input);
				setLimit('min', c.lower, $input);
			}
			
			function setLimit(type, unixt, $input){
				p = $input.pickadate('picker');
				d = moment.unix(unixt).format('YYYY-MM-DD');
				date = d.split("-");
				date[1]--;
				p.set(type, date);
			}
			break;
		case('progress'):
			var t = me.meta.type;
			if(t.exist && t.value == 'medium' || t.value == 'high') {
				$content.append(me.generateContent(ptype));
			}else{
				$e = $('<input type="text" placeholder="Progress" maxlength="3" style="width:35px;"/>');
				$e.val(Math.round(p.value));				
				$e.keyup(function(e){
					if(e.keyCode == 13){
						var v = $(this).val();
						if(isInt(v) && 0<=v && v<=100 || v == ''){
							me.expapi.lockAll('Set Task Progress...', me);
							me.wtapi.setProgress(me.title, v, function(data){
				        		me.expapi.notifyAll(data.wttasks.update, me);
				        	});
							me.setLocal(ptype, v);
							me.closeAllEdits();
							WTTracker.track({
								component: WTTracker.c.metadata,
								subcomponent: WTTracker.s.progress,
								actiontype: WTTracker.t.set,
								action: 'set progress',
								taskId: me.title,
								value: v
							});	
						}
					}
				});
				$content.append($e);
				$e.focus();
			}
		
			function isInt(value){
				var er = /^-?[0-9]+$/;
				return er.test(value);
			}
			break;
		case('participants'):
			$edit = $('<div></div>');
			$.each(p.value, function(k,v){
				$edit.append(appendParticipant(v));
			});			
			$in = $('<input type="text" placeholder="Participant" style="width:100px;"/>');
			$in.autocomplete2({lookup: me.expapi.personSuggestion()});			
			$in.keyup(function(e){
				if(e.keyCode == 13){	
					var v = $(this).val();
					if(v != '' && me.meta.participants.value.indexOf(v) == -1){
						$in.val('');
						me.addLocal(ptype, v);
						$in.before(appendParticipant(v));
						me.expapi.lockAll('Add Task Participant...', me);
						me.wtapi.addParticipant(me.title, v, function(data){
			        		me.expapi.notifyAll(data.wttasks.update, me);
			        	});						
						$in.autocomplete2().dispose();
						$in.autocomplete2({lookup: me.expapi.personSuggestion()});
						WTTracker.track({
							component: WTTracker.c.metadata,
							subcomponent: WTTracker.s.participants,
							actiontype: WTTracker.t.add,
							action: 'add participant',
							taskId: me.title,
							value: v
						});	
					}
				}
			});
			$edit.append($in);
			$content.append($edit);
			$in.focus();

			function appendParticipant(participant){
				$a = $('<a class="plink">'+participant+'</a>');
				if(!me.expapi.personExist(participant))
					$a.addClass('new');					
				$item = $('<div class="participant"></div>').append($a);
				$del = $('<div class="del">&nbsp;</div>');
				$del.click(function(){
					$participant = $(this).closest('.participant');
					var v = $participant.find('.plink').text();
					me.expapi.lockAll('Remove Task Participant...', me);
					me.wtapi.removeParticipant(me.title, participant, function(data){
		        		me.expapi.notifyAll(data.wttasks.update, me);
		        	});
					me.removeLocal(ptype, v);
					$participant.remove();
					WTTracker.track({
						component: WTTracker.c.metadata,
						subcomponent: WTTracker.s.participants,
						actiontype: WTTracker.t.del,
						action: 'del participant',
						taskId: me.title,
						value: v
					});	
				});
				$item.append($del)
				$item.append(',')
				return $item;
			}	
			
			
			break;
		case('expertise'):
			$edit = $('<div></div>');
			$.each(p.value, function(k,v){
				$edit.append(appendExpertise(v));
			});			
			$in = $('<input type="text" placeholder="Expertise" style="width:100px;"/>');
			$in.autocomplete2({lookup: me.expapi.expertiseSuggestion()});
			$in.keyup(function(e){
				if(e.keyCode == 13){	
					var v = $(this).val();
					if(v != '' && me.meta.expertise.value.indexOf(v) == -1){
						$in.val('');	
						me.addLocal(ptype, v);
						$in.before(appendExpertise(v));	
						me.expapi.lockAll('Add Task Expertise...', me);
						me.wtapi.addExpertise(me.title, v, function(data){
			        		me.expapi.notifyAll(data.wttasks.update, me);
			        	});									
						$in.autocomplete2().dispose();
						$in.autocomplete2({lookup: me.expapi.expertiseSuggestion()});	
						WTTracker.track({
							component: WTTracker.c.metadata,
							subcomponent: WTTracker.s.expertise,
							actiontype: WTTracker.t.add,
							action: 'add expertise',
							taskId: me.title,
							value: v
						});		
					}
				}
			});
			$edit.append($in);
			$content.append($edit);	
			$in.focus();

			function appendExpertise(expertise){
				$item = $('<div class="expertise"><span>'+expertise+'</span></div>');
				$del = $('<div class="del">&nbsp;</div>');
				$del.click(function(){
					$expertise = $(this).closest('.expertise');
					v = $expertise.find('span').text();
					me.expapi.lockAll('Remove Task Expertise...', me);
					me.wtapi.removeExpertise(me.title, expertise, function(data){
		        		me.expapi.notifyAll(data.wttasks.update, me);
		        	});
					me.removeLocal(ptype, v);
					$expertise.remove();
					WTTracker.track({
						component: WTTracker.c.metadata,
						subcomponent: WTTracker.s.expertise,
						actiontype: WTTracker.t.del,
						action: 'del expertise',
						taskId: me.title,
						value: v
					});		
				});
				$item.append($del)
				return $item;
			}					
			break;
	}	

	var v = me.meta[ptype].value;
	if(ptype == 'expertise' || ptype == 'participants')
		v = JSON.stringify(v);
	WTTracker.track({
		component: WTTracker.c.metadata,
		subcomponent: ptype,
		actiontype: WTTracker.t.nav,
		action: 'open edit '+ptype,
		taskId: me.title,
		value: v
	});

};

WTTaskMetaData.prototype.removeLocal = function(type, value){
	var me = this;
	me.meta[type].value = me.meta[type].value.filter(function(e){ return e != value; });
	me.meta[type].exist = me.meta[type].value.length>0;
	me.updateIcons();
};

WTTaskMetaData.prototype.addLocal = function(type, value){
	var me = this;
	if(me.meta[type].value.indexOf(value)==-1){
		me.meta[type].value.push(value);
		me.meta[type].exist = true;
		me.updateIcons();
	}
};

WTTaskMetaData.prototype.setLocal = function(type, value){
	var me = this;
	me.meta[type].value = value;
	me.meta[type].exist = !value == '';
	me.updateIcons();
};

WTTaskMetaData.prototype.updateIcons = function(){
	var me = this;
	me.$table.find('.wt-row').each(function(){
		$t = $(this);		
		ptype = $t.attr('property');
		property = me.meta[ptype];
		var t = me.meta.type;
		var fade = !property.exist || ptype == 'progress' && t.exist && (t.value == 'medium' || t.value == 'high');
		if(fade)
			$t.addClass('wt-fade');
		else
			$t.removeClass('wt-fade');
		$t.find('.wt-cell.wt-icon').css('background-image', 'url('+me.generateIconPath(ptype, fade, property.warning.exist)+')');
	});	
}




WTTaskMetaData.prototype.generateContent = function(ptype) {
	var me = this;
	var property = this.meta[ptype];
	var type = this.meta.type;
	$content = $('<div></div>');
	if(property.exist || ptype == 'progress' && type.exist 
			&& (type.value == 'medium' || type.value == 'high')){
		$content.html(me.typeToValue(ptype));
	}else{
		$content.html('Not defined!');
		$content.addClass('notexist');	
	}
	return $content
};

WTTaskMetaData.prototype.typeToValue = function(ptype) {
	var me = this;
	p = this.meta[ptype];
	switch(ptype){
		case('start'):
		case('target'): 	
			return moment.unix(p.value).format('Do MMM YYYY');
		case('progress'): 	
			type = this.meta.type.value;
			lock ='';
			if(type == 'medium' || type == 'high')
				lock ='<img src="'+this.generateLockIconPath()+'" width="16" height="16" />';				
			return Math.round(p.value)+'%'+lock;
		case('owner'): 		
			$a = $('<a>'+p.value+'</a>');
			$a.attr('href', p.value);
			if(!me.expapi.personExist(p.value))
				$a.addClass('new');
			$a.click(function( event ) {
				event.stopPropagation();
			});
			return $a;
		case('participants'):
			$p = $('<div></div>');
			$.each(p.value, function(k, v){
				var comma = p.value.length-1 > k ? ',': '';
				$a = $('<a>'+v+'</a>');
				$a.attr('href', v);
				if(!me.expapi.personExist(v))
					$a.addClass('new');
				$a.click(function( event ) {
					event.stopPropagation();
				});
				$p.append($a);
				$p.append('<span>'+comma+' </span>');
			});
			return $p;
		case('expertise'):
			$p = $('<div></div>');
			$.each(p.value, function(k,v){
				$a = $('<div class="expertise">'+v+'</div>');
				$p.append($a);
			});
			return $p;
		default: 
			return p.value;
	}	
};

WTTaskMetaData.prototype.typeToLabel = function(ptype) {
	var m = '<span class="mandatory">M</span>';
	switch(ptype){
		case('owner'): 		return 'Owner'+m;
		case('type'): 		return 'Type'+m;
		case('progress'): 	return 'Progress'+m;
		case('start'): 		return 'Start date'+m;
		case('target'): 	return 'Target date'+m;
		case('expertise'): 	return 'Expertise';
		case('participants'):return 'Participants';
	}
};

WTTaskMetaData.prototype.generateIconPath = function(icon, fade, warning) {
	return this.baseurl+'/extensions/WorkflowTasks/includes/core/api/WTIconAPI.php?icon='+icon+'&fade='+fade+'&warning='+warning;
};

WTTaskMetaData.prototype.generateLockIconPath = function() {
	return this.baseurl+'/extensions/WorkflowTasks/includes/core/api/WTIconAPI.php?icon=lock';
};

WTTaskMetaData.prototype.lock = function(message) {
	this.$item.mask(lpMsg(message));
};


