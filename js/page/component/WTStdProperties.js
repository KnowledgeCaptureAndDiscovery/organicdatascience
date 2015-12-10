var WTStdProperties = function(title, wtfacts, stdprops, util, api ) {
	this.title = title;
	this.wtfacts = wtfacts;
	this.stdprops = stdprops;
	this.util = util;
	this.api = api;
};

WTStdProperties.prototype.display = function($item) {
	this.$item = $item;
	this.$item.empty();
	this.$item.unmask();
	this.generateTable();
	var me = this;
	$('body').click(function() {
		me.closeAllEdits();
	});
};

WTStdProperties.prototype.notify = function() {
	this.display();
};

WTStdProperties.prototype.generateTable = function() {
	var me = this;

	// Add children and levels to properties
	for(var pname in this.stdprops) {
		var prop = this.stdprops[pname];
		if(prop.parent) {
			var pprop = this.stdprops[prop.parent];
			if(pprop.children)
				pprop.children.push(prop);
			else 
				pprop.children = [prop];
		}
	}
	for(var pname in this.stdprops) {
		var prop = this.stdprops[pname];
		prop.level = 0;
		var curprop = prop;
		while(curprop.parent) {
			prop.level++;
			curprop = this.stdprops[curprop.parent];
		}
	}

	me.$table =  $('<div class="wt-table"></div>');
	me.appendHeadingRow();
	me.$item.append(me.$table);
	for(var pname in this.stdprops) {
		var property = this.stdprops[pname];
		me.appendPropertyRows(property, pname, 0);
	}
	me.appendNotesRow();
	me.updateIcons();
};

WTStdProperties.prototype.appendPropertyRows = function(property, pname, lvl) {
	if(property.level != lvl)
		return;
	$row = this.appendRow(property, pname)
	if(property.level) {
		$label = $row.find('.wt-label');
		var pad = '';
		for(var i=0; i<(property.level-1)*5; i++) 
			pad += '&nbsp;';
		pad += '&#8627;';
		$label.html(pad + ' ' + $label.html());
	}
	if(property.children) {
		for(var i=0; i<property.children.length; i++) {
			var cprop = property.children[i];
			this.appendPropertyRows(cprop, cprop.label, lvl+1);
		}
	}
};

WTStdProperties.prototype.appendRow = function(property, pname) {
	var me = this;
	var $row = $('<div class="wt-row"></div>');
	$row.attr('property', pname);
	me.$table.append($row);
	$row.click(function(e){	
		$t = $(this);
		if(!$t.hasClass('edit') && wtuid){
			me.closeAllEdits();
			$t.addClass('edit');
			$c = $t.find('.wt-content');
			$c.html('');
			var index = 0;
			var valobj = me.propValue(pname, index);
			while(valobj) {
				$in = me.generateEdit($c, pname, valobj);
				valobj = me.propValue(pname, ++index);
			}
			if(property.multivalues || !index) {
				$in = me.generateEdit($c, pname, null, true);
				if(index)
					$in.attr('placeholder', 'Add another '+$in.attr('placeholder'));
			}
		}
		e.stopPropagation();
	});	
	me.appendIconCell($row, pname, property);
	me.appendLabelCell($row, pname, property);
	me.appendContentCell($row, pname);
	me.appendAuthorCell($row, pname);
	return $row;
};

WTStdProperties.prototype.generateCancelButton = function(pname, valobj) {
	var me = this;
	var oldv = (valobj && valobj.val) ? valobj.val : null;
	if(!oldv) return null;
	var delhref = $('<a class="lodlink"><i class="fa fa-times-circle delbutton"></i></a>');
	delhref.click( function(e) {
		var p = me.stdprops[pname];
		me.$item.mask(lpMsg('Removing Fact..'));
		me.api.removeFact( me.title, p.label, oldv, function(response){
			me.$item.unmask();
			if(!response || !response.wtfacts || !response.wtfacts.facts) return; 
			me.wtfacts = response.wtfacts.facts;
			me.closeAllEdits();
			me.updateIcons();
		});
	});
	return delhref;
};

WTStdProperties.prototype.appendHeadingRow = function() {
	var me = this;
	var heading  = '<div class="heading">';
		heading += '  <span><b>Standard Properties</b></span>'
		heading += '</div>';
	me.$item.append($(heading));
};

WTStdProperties.prototype.appendNotesRow = function() {
	var me = this;
	var notes  = '<div class="notes">';
		notes += '  <span><b>Legend: </b></span>'
		notes += '	<span style="color:#cccccc; vertical-align: -1px; font-size:17px;">&#9632;</span> Not defined, ';
		notes += '	<span style="color:#b7db7a; vertical-align: -1px; font-size:17px;">&#9632;</span> Valid, ';
		notes += '</div>';
	me.$item.append($(notes));
};

WTStdProperties.prototype.closeAllEdits = function() {
	var me = this;
	me.$table.find('.wt-row').each(function(k, row){		
		$row = $(row);
		$row.removeClass('edit');
		var pname = $row.attr('property');
		var index = 0;
		$c = $row.find('.wt-content');
		$a = $row.find('.wt-author');
		$c.html(''); $a.html('');
		$content = me.generateContent(pname, index);
		while($content) {
			$c.append($content);
			$a.append(me.getAuthorCredit(pname, index));
			$content = me.generateContent(pname, ++index);
		}
	});
};

WTStdProperties.prototype.appendIconCell = function($row, pname, property) {
	$cell = $('<div class="wt-cell wt-icon"></div>');
	var icon = property.icon ? property.icon : 'fa-tag';
	var iconhtml = '<i class="fa '+icon+' fa-lg"></i> ';
	$cell.html(iconhtml);
	$row.append($cell);
};

WTStdProperties.prototype.appendLabelCell = function($row, pname, property) {
	$cell = $('<div class="wt-cell wt-label"></div>');
    var lprop = property.label;
    var propcls = property.exists ? 'lodlink' : 'lodlink new';

    var wgScriptPath = mw.config.get('wgScriptPath');
    var propuri = wgScriptPath + '/index.php/Property:' + property.label;
    $propentity = $('<a href="' + propuri + '" class="'+propcls+'">' + lprop + '</a>');
	$propentity.click(function(e) { e.stopPropagation(); });
	$cell.append($propentity);
	//$cell.html(this.typeToLabel(pname));
	$row.append($cell);
}

WTStdProperties.prototype.appendContentCell = function($row, pname) {	
	$cell = $('<div class="wt-cell wt-content"></div>');
	//$cell.append(this.generateContent(pname));	
	var index = 0;
	$content = this.generateContent(pname, index);
	while($content) {
		$cell.append($content);
		$content = this.generateContent(pname, ++index);
	}
	$row.append($cell)
};

WTStdProperties.prototype.appendAuthorCell = function($row, pname) {	
    $cell = $('<div class="wt-cell wt-author"></div>');
	//$cell.html(this.getAuthorCredit(pname));
	var index = 0;
	$auth = this.getAuthorCredit(pname, index);
	while($auth) {
		$cell.append($auth);
		$auth = this.getAuthorCredit(pname, ++index);
	}
	$row.append($cell)
};

WTStdProperties.prototype.getAuthorCredit = function(pname, index) {	
	var html = "";
	var valobj = this.propValue(pname, index);
	if(valobj && valobj.author)
		html = "<div>(By "+valobj.author+")</div>";
	return html;
};

WTStdProperties.prototype.generateEdit = function($content, pname, valobj, addop) {
	var me = this;
	var p = me.stdprops[pname];

	$in = $('<input type="text"/>');
	switch(p.type) {
		case('_wpg'):
			var placeholder = p.category ? p.category : '';
			$in = $('<input type="text" placeholder="'+placeholder+'"/>');
			if(p.category) {
				$in.autocomplete({
					delay:300,
					minLength:1,
					source: function(request, response) {
						var item = this;
						me.api.getSuggestions(request.term, p.category, function(sug) {
							response.call(this, sug.wtsuggest.suggestions);
						});
					},
					select: function(e) {
						e.stopPropagation();
					}
				});
			}
			break;
		case('_num'):
			$in = $('<input type="text" placeholder="Number"/>');
			break;
		case('_date'):
			$in = $('<input type="text" placeholder="Date"/>');
			break;
		case('_uri'):
			$in = $('<input type="text" placeholder="URL"/>');
			break;
		case('_txt'):
			$in = $('<input type="text" placeholder="Text"/>');
			break;
		case('_ema'):
			$in = $('<input type="text" placeholder="Email"/>');
			break;
		case('_boo'):
			$in = $('<input type="text" placeholder="Boolean"/>');
			break;
	}
	if(!valobj && !addop)
		return null;

	if(valobj)
		$in.val(valobj.val);
	$in.keyup(function(e){
		if(e.keyCode == 13){
			var v = e.currentTarget.value;
			var oldv = (valobj && valobj.val) ? valobj.val : null;
			if(addop) {
				if(v) {
					me.$item.mask('Adding '+p.label);
					me.api.addFact(me.title, p.label, v, function(response){
						me.$item.unmask();
						if(!response || !response.wtfacts || !response.wtfacts.facts) return; 
						me.wtfacts = response.wtfacts.facts;
						me.closeAllEdits();
						me.updateIcons();
        			});
				}
			}
			else if(v != oldv) {
				me.$item.mask('Setting '+p.label);
				me.api.replaceFact(me.title, p.label, v, oldv, function(response){
					me.$item.unmask();
					if(!response || !response.wtfacts || !response.wtfacts.facts) return; 
					me.wtfacts = response.wtfacts.facts;
					me.closeAllEdits();
					me.updateIcons();
        		});
			}
		}
	});
	$div = $('<div style="white-space:nowrap"></div>');
	if(!addop)
		$div.append(this.generateCancelButton(pname, valobj)).append(' ');
	else	
		$div.append('<i class="fa fa-plus"></i>').append(' ');
	$div.append($in);
	$content.append($div);
	$in.focus();
	return $in;
};

WTStdProperties.prototype.generateContent = function(pname, index) {
	var me = this;
	var property = this.stdprops[pname];
	var valobj = me.propValue(pname, index);
    var wgScriptPath = mw.config.get('wgScriptPath');
	$content = $('<div></div>');
	if(valobj) {
    	$valentity = $("<span></span>");
		$valentity.html(valobj.text);
    	if(valobj.type == 'WikiPage') {
        	var valcls = valobj.exists ? '' : 'new';
    		var valuri = wgScriptPath + '/index.php/' + valobj.key;
        	$valentity = $("<a href='"+valuri+"' class='"+valcls+"'>"+valobj.val.replace(/_/g,' ')+"</a>");
			$valentity.click(function(e) { e.stopPropagation(); });
    	}
    	else if(valobj.type == 'Uri') {
        	var valtext = valobj.val.replace(/Www/, 'www');
        	$valentity = $("<a href='"+valobj.val+"'>"+valtext+"</a>");
			$valentity.click(function(e) { e.stopPropagation(); });
    	}
		$content.html('');
		$content.append($valentity);
	} else if(!index) {
		$content.html('Not defined!');
		$content.addClass('notexist');	
	} else if(index) {
		return null;
	}
	return $content;
};

WTStdProperties.prototype.propValue = function(pname, index) {
	var me = this;
	if(!index) index=0;
	var valobj = this.wtfacts[pname];
	if(valobj && valobj.values && valobj.values.length > index)
		return valobj.values[index];
	return null;
};

WTStdProperties.prototype.typeToLabel = function(pname) {
	return pname.replace(/_/g, ' ');
};

WTStdProperties.prototype.lock = function(message) {
	this.$item.mask(lpMsg(message));
};

WTStdProperties.prototype.updateIcons = function(){
	var me = this;
	me.$table.find('.wt-row').each(function(){
		$t = $(this);		
		var pname = $t.attr('property');
		var valobj = me.propValue(pname);
		var fade = !valobj;
		if(fade)
			$t.addClass('wt-fade');
		else
			$t.removeClass('wt-fade');
	});	
};

