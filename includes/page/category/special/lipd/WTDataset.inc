<?php

class WTDataset extends WTBase {
	function __construct($title) {
		parent::__construct($title);
	}

	function modifyWikiPage($out) {
		parent::modifyWikiPage($out);
		global $wgScriptPath;
		$old = $out->getHTML();
		$out->clearHTML();
		
		# -- Add the Download LiPD button
		$out->addHTML('<div style="margin-bottom:5px"><a class="lodbutton" href="' .
			$wgScriptPath . '/index.php/Special:WTLiPD?op=export&lipdid=' .
			$this->title.'">Download LiPD</a></div>');

		# -- Add Existing Markup Here
		$out->addHTML( $old );
	}
}

