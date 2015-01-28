<?php

class WTProperty extends WTBase {
	function __construct($title) {
		parent::__construct($title);
	}

	function setJavascriptVariables($out) {
	}

	function includeJSHeaders($out, $wgDir) {
	}

	function modifyWikiPage($out) {
		$old = $out->getHTML();
		$out->clearHTML();

		# -- Add Information Box
		$out->addHTML('<div class="wt-infobox">');
		$out->addHTML('Add <b style="color:maroon">[[has type::&lt;Type&gt;]]</b> in the wiki text for this property to specify Property Range');
		$out->addHTML('<ul><li><a href="http://semantic-mediawiki.org/wiki/Property:Has_type">See Documentation Here</a>.<li><a href="http://semantic-mediawiki.org/wiki/Special:Types">List of all types</a>.</ul>');
		$out->addHTML('</div>');

		# -- Add Existing Markup Here
		$out->addHTML($old);

	}

}

