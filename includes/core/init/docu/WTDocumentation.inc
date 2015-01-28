<?php


class WTDocumentation{
   	   	
	public static function create(){	
		set_time_limit (60*60);
		$rootPage = self::loadXMLSchema()->page;
		self::createDocuPages($rootPage, $rootPage);
	}
	
   	private static function loadXMLSchema(){
   		global $wgCanonicalServer;
		global $wgScriptPath;
		$url = $wgCanonicalServer.$wgScriptPath.'/extensions/WorkflowTasks/includes/docu/documentation.xml';
   		return simplexml_load_file($url);
   	}
	
	private static function createDocuPages($rootPage, $page){
		$attr = $page->attributes();
		$title = $attr->title.'';
		$content = self::getPageContent($page);
		self::createDocuPage($rootPage, $title, $content);
		if($subpages = $page->subpages){
			foreach ($subpages->children() as $subpage)
				self::createDocuPages($rootPage, $subpage);
		}	
	}
	
	private static function getPageContent($page){
		$content = $page->content->asXML();
		$content = str_replace('<content>', '', $content);
		$content = str_replace('</content>', '', $content);
		return str_replace("\t", '', $content);	
	}
	
	private static function createDocuPage($rootPage, $title, $content){		
      	$content = self::createPageContent($content, $rootPage, $title);
      	$article = new Article(Title::newFromText($title));
		$summary = "Creating new page with Category: Docu";
		$article->doEdit($content, $summary);
	}
	
	private static function createPageContent($content, $rootPage, $selectedTitle){
		$menu = self::createNav($rootPage, 0, $selectedTitle);
		$html  = '[[Category:Docu]] ';
		$html .= '<!--Do not edit this page manually the docu is generated via XML Script and will override manual edits!-->';
		$html .= '<table id="wt-documentation" cellspacing="0" cellpadding="0">';
		$html .= '	<tr>';
		$html .= '		<td class="content">'.$content.'&nbsp;</td>';
		$html .= '		<td class="menu">'.$menu.'</td>';
		$html .= '	</tr>';
		$html .= '</table>';
		return $html;
	}
	
	private static function createNav($page, $level, $selectedTitle){
		$ret = self::createNavItem($page, $level, $selectedTitle);
		if($subpages = $page->subpages)		
			foreach ($subpages->children() as $subpage)
				$ret .= self::createNav($subpage, $level+1, $selectedTitle);
		return $ret;
	}
	
	private static function createNavItem($page, $level, $selectedTitle){
		$title = $page->attributes()->title.'';		
		$selectedTitle == $title ? $sel = 'selected' : $sel = '';
		return '<div class="item '.$sel.'" style="'.self::style($level).'">'.$title.'</div>';
	}
	
	private static function style($level){
		$mleft = 5+$level*20;
		switch ($level){
			case(0): return 'padding-left:'.$mleft.'px; font-size:14px; font-weight:bold;';
			case(1): return 'padding-left:'.$mleft.'px; font-size:12px; font-weight:bold;';
			default: return 'padding-left:'.$mleft.'px; font-size:10px;';
		}	
	}
}