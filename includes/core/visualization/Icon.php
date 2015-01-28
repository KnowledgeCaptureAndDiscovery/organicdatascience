<?php

abstract class Icon{
		
	abstract protected function renderSvg();
	
	public function render(){			
		$svg  = '<?xml version="1.0" encoding="UTF-8" standalone="no"?>';
		$svg .= '<svg width="'.$this->getIconWidth().'" height="'.$this->getIconHeight().'" version="1.1" xmlns="http://www.w3.org/2000/svg">';
		$svg .= 	$this->renderSvg();
		$svg .= '</svg>';
		
		header('Content-Encoding: gzip', true);
		header('Content-Type: image/svg+xml', true);
		header('Cache-Control: max-age=3600, must-revalidate');	
		echo gzencode($svg, 9);
	}
	
	protected function setIconWidth($width){
		$this->iconWidth=$width;
	}
	
	private function getIconWidth(){
		if(isset($this->iconWidth))
			return $this->iconWidth;
		return "100%";
	}
	
	protected function setIconHeight($height){
		$this->iconHeight=$height;
	}
	
	private function getIconHeight(){
		if(isset($this->iconHeight))
			return $this->iconHeight;
		return "100%";
	}
	
	
}

