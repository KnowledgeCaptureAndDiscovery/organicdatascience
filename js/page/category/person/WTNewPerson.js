var WTNewPerson = function(title, details, util, api ) {
	this.title = title;
	this.details = details;
	this.util = util;
	this.api = api;
};

WTNewPerson.prototype.display = function( item ) {
	var me = this;
	item.data('data', me.details);
	var message = $('<div style="padding:5px;color:red;font-weight:bold"></div>').append($('<b>Uh oh. You don\'t have a Person page. Create new ?</b>'));
	item.append(message);

	var plist = $.map( me.details.persons, function(v,k) { return k; } );
	var $userlist = $('<input type="text" placeholder="Select an existing person (or write your own name)" style="width:50%"/>');
	$userlist.autocomplete({
		delay:300,
		minLength:0,
		highlightClass:'none',
		source: plist
	});

	$userlist.keyup(function( e ) {
		if(e.keyCode == 13){
			var name = $userlist.val();
			console.log(name);
			item.mask(lpMsg('Create Person_' + wgCore+' page..'));
			me.api.createPageWithCategory( name, "Person_" + wgCore, function(response) {
				if(!response || !response.wtfacts) return;
				if(response.wtfacts.result == 'Success') {
					me.api.addFact( name, "Has_User_ID", me.details.wikiuser.id, function(response) {
						if(!response || !response.wtfacts) return;
						if(response.wtfacts.result == 'Success') {
							window.location = name;
						}
					});
				}
			});
		}
	});

	item.append($userlist);
};

