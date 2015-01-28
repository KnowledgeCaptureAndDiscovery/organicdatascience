var WTSimpleTasks = function(title, tree, util, api ) {
	this.title = title;
	this.tree = tree;
	this.util = util;
	this.api = api;
 	this.cat = wtcategories['Procedure'] ? 'Procedure' : 'Task';
};


WTSimpleTasks.prototype.getListItem = function( list, subdata, answers, is_sub ) {
	var sub_li = $('<li></li>');

	var me = this;
	if(!is_sub && wtuid) {
		var delhref = $('<a class="lodlink"><i class="fa fa-times-circle fa-lg delbutton"></i></a>');
		// Minus [-] link's event handler
		delhref.click( function(e) {
			list.mask(lpMsg('Removing SubTask..'));
			me.api.removeSimpleSubTask( me.title, subdata.text, function(resp) {
				list.unmask();
				if(!resp || !resp.wtfacts) return;
				if(resp.wtfacts.result == 'Success') {
					sub_li.remove();
				}
			});
		});
		sub_li.append(delhref).append(' ');
	}

	var sub_cls = subdata.exists ? '' : 'new';
	sub_li.append($('<a class="'+sub_cls+'" href="'+subdata.key+'"></a>').append(subdata.text));

	if(answers && answers.length) {
		sub_li.append(' (Answer: ');
		var i=0;
		$.each(answers, function(ind, ans) {
			if(i) sub_li.append(', ');
			var ans_cls = ans.item.exists ? '' : 'new';
			sub_li.append($('<a class="'+ans_cls+'" href="'+ans.item.key+'"></a>').append(ans.item.text));
			i++;
		});
		sub_li.append(')');
	}

	return sub_li;
};

WTSimpleTasks.prototype.getTree = function( item, data, is_sub ) {
	var list = $('<ul class="wt-tree"></ul>');

	var me = this;

	if(!is_sub) {
		var ival = $('<input style="width:60%" type="text" />');
		this.util.registerSuggestions(ival, this.cat, this.api);

		var igo = $('<a class="lodbutton">' + lpMsg('Go') + '</a>');
		var icancel = $('<a class="lodbutton">' + lpMsg('Cancel') + '</a>');
		var addsub_li = $('<li></li>');
		addsub_li.append(ival).append(igo).append(icancel).hide();
		list.append(addsub_li);

		icancel.click(function( e ) {
			ival.val('');
			ival.data('val','');
			addsub_li.hide();
		});

		igo.click(function(e) { localAdd() });
		ival.keyup(function(e) {
			if(e.keyCode == 13) { localAdd(); }
		});

		function localAdd() {
			var val = ival.data('val') ? ival.data('val') : ival.val();
			addsub_li.hide();
			if(!val) return; 
			ival.val('');
			ival.data('val','');
	
			item.mask(lpMsg('Adding SubTask..'));
			me.api.addSimpleSubTask( me.title, val, function(response) {
				item.unmask();
				if(!response || !response.wtfacts || !response.wtfacts.newdetails) return; 
				if(response.wtfacts.result == 'Success') {
					var sub = response.wtfacts.newdetails;
					var sub_li = me.getListItem(item, sub.item, sub.Answers, is_sub);
					var sublist = me.getTree(sub_li, sub, true);
					list.append(sub_li.append(sublist));
				}
			});
		}
	}

	if(data) {
		$.each(data.SubTasks, function(ind, sub) {
			var sub_li = me.getListItem(item, sub.item, sub.details.Answers, is_sub);
			var sublist = me.getTree(sub_li, sub.details, true);
			list.append(sub_li.append(sublist));
		});
	}

	item.data('list', list);
	return list;
};


WTSimpleTasks.prototype.appendParents = function( item ) {
	if( !this.tree.Parents || !this.tree.Parents.length ) return;

	var header = $('<div class="heading"></div>').append($('<b>Parent Tasks</b>'));
	item.append(header);
	var wrapper = $('<div style="padding:5px"></div>');

	var list = $('<ul style="margin-bottom:8px"></ul>');
	$.each(this.tree.Parents, function(ind, par) {
		var par_cls = par.exists ? '' : 'new';
		var link = $('<a class="'+par_cls+'" href="'+par.key+'"></a>').append(par.text);
		list.append($("<li></li>").append(link));
	});

	wrapper.append(list);
	item.append(wrapper);
}

WTSimpleTasks.prototype.display = function( item ) {
	var me = this;

	item.data('data', me.tree);

	var list = me.getTree( item, me.tree );

	var addsub_link = '';
	if(wtuid) {
		addsub_link = $('<a class="lodlink"><i class="fa fa-plus-circle fa-lg"></i></a>');
		addsub_link.click(function( e ) {
			list.find('li:first').css('display', '');
		});
	}

	this.appendParents(item);
 	var headtitle = "Sub" + this.cat + "s";
	var header = $('<div class="heading"></div>').append($('<b>'+headtitle+'</b>')).append(' ').append(addsub_link);
	item.append(header);
	var wrapper = $('<div style="padding:5px"></div>');
	wrapper.append(list);
	item.append(wrapper);
};

