var WTAnswers = function(title, tree, util, api ) {
	this.title = title;
	this.tree = tree;
	this.util = util;
	this.api = api;
};


WTAnswers.prototype.getListItem = function( list, ansdata ) {
	var ans_li = $('<li></li>');

	var me = this;
	if(wtuid) {
		var delhref = $('<a class="lodlink"><i class="fa fa-times-circle fa-lg delbutton"></i></a>');
		delhref.click( function(e) {
			list.mask(lpMsg('Removing Answer..'));
			me.api.removeAnswer( me.title, ansdata.text, function(resp) {
				list.unmask();
				if(!resp || !resp.wtfacts) return;
				if(resp.wtfacts.result == 'Success') {
					ans_li.remove();
				}
			});
		});
		ans_li.append(delhref).append(' ');
	}

	var ans_cls = ansdata.exists ? '' : 'new';
	ans_li.append($('<a class="'+ans_cls+'" href="'+ansdata.key+'"></a>').append(ansdata.text));

	return ans_li;
};

WTAnswers.prototype.getList = function( item, data ) {
	var list = $('<ul></ul>');

	var me = this;

	var ival = $('<input style="width:60%" type="text" />');
	this.util.registerSuggestions(ival, 'Answer', this.api);

	var igo = $('<a class="lodbutton">' + lpMsg('Go') + '</a>');
	var icancel = $('<a class="lodbutton">' + lpMsg('Cancel') + '</a>');

	var addans_li = $('<li></li>').append($('<div style="width:24px"></div>'));
	addans_li.append(ival).append(igo).append(icancel).hide();
	list.append(addans_li);

	icancel.click(function( e ) {
		ival.val('');
		ival.data('val','');
		addans_li.hide();
	});

	igo.click(function(e) { localAddAnswer() });
	ival.keyup(function(e) {
		if(e.keyCode == 13) { localAddAnswer(); }
	});

	function localAddAnswer() {
		var val = ival.data('val') ? ival.data('val') : ival.val();
		addans_li.hide();
		if(!val) return; 
		ival.val('');
		ival.data('val','');

		item.mask(lpMsg('Adding Answer..'));
		me.api.addAnswer( me.title, val, function(response) {
			item.unmask();
			if(!response || !response.wtfacts || !response.wtfacts.newdetails) return; 
			if(response.wtfacts.result == 'Success') {
				var ans = response.wtfacts.newdetails;
				var ans_li = me.getListItem(item, ans.item);
				list.append(ans_li);
			}
		});
	}

	if(data) {
		$.each(data.Answers, function(ind, ans) {
			var ans_li = me.getListItem(item, ans.item);
			list.append(ans_li);
		});
	}

	item.data('list', list);
	return list;
};


WTAnswers.prototype.display = function( item ) {
	var me = this;

	item.data('numchecked', 0);
	item.data('checked_data', []);
	item.data('data', me.tree);

	var list = me.getList( item, me.tree );


	var addans_link = '';
	if(wtuid) {
		addans_link = $('<a class="lodlink"><i class="fa fa-plus-circle fa-lg"></i></a>');
		addans_link.click(function( e ) {
			list.find('li:first').css('display', '');
		});
	}

	var header = $('<div class="heading"></div>').append($('<b>Answers</b>')).append(' ').append(addans_link);
	item.append(header);
	var wrapper = $('<div style="padding:5px"></div>');
	wrapper.append(list);
	item.append(wrapper);
};

