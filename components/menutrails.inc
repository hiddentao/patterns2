<?php
/**
 * Component for updating menutrails configuration.
 * @author Ramesh Nair (hiddentao)
 */


function menutrails_patterns($op, $id = null, &$data = null) {
  switch($op) {
    // Return the valid tags that this component can prepare and process
    case 'tags':
      return array('menu_trail');
    break;

    // Return a list of forms/actions this component can handle
    case 'actions':
      return array(
        'menutrails_settings_form' => t('Menu Trails: Add/configure trails'),
      );
    break;

    // Return a summary of an action
    case 'summary':
      if ('menutrails_settings_form' == $id && isset($data['type'])) {
        return t('Configuring trail for node type: %name', array('%name' => $data['type']));
      }
    break;

    // Prepare data for processing
    case 'prepare':
    break;

    // Pre validate actions
    case 'pre-validate':
    	if (!module_exists('menutrails')) {
    		return t('Menu Trails module must be enabled.');
    	}
      if (!isset($data['menu']) || !isset($data['menu']['parent']) || !isset($data['menu']['item'])) {
        return t('Valid "menu" (with "parent" and "item") required.');
      }
      if (!isset($data['type'])) {
        return t('You must specify a "type" to associate with the menu.');
      }
    break;

    // Return the form_id('s) for each action
    case 'form_id':
    	return 'menutrails_settings_form';
    	
    break;

    // Prepare for valid processing of this type of component
    case 'build':
    	if ('menutrails_settings_form' == $id) {
        
    		// node type
        $types = node_get_types('types');
        $type_internal_name = '';
        foreach ($types as $type => $info) {
        	if ($info->name == $data['type']) {
        		$type_internal_name = $type;
        		break;
        	} else if ($type == $data['type']) {
            $type_internal_name = $type;
            break;
        	}
        }    		
    		
        // menu item
	      $menus = menu_get_menus(TRUE);
	      $menu_internal_name = '';
	      foreach ($menus as $name => $title) {
	        if ($title == $data['menu']['parent']) {
	        	$menu_internal_name = $name;
	          break;
	        } else if ($name == $data['menu']['parent']) {
          	$menu_internal_name = $name;
            break;
          }
	      }
	      $tree = menu_tree_all_data($menu_internal_name);
	      $menu_item = _menutrails_patterns_get_menu_item($tree, $data['menu']['item']);

	      // update params
	      $data['menutrails_node_types'][$type_internal_name] = $menu_item['link']['href'];
      }
      
      return $data;
    break;

    // Validate the values for an action before running the pattern
    //
    // Check here rather than in pre-validate in case we're adding 
    // roles or filters in the same pattern.
    //
    case 'validate':
    	
    	// check node type
    	$types = node_get_types('types');
    	$found = FALSE;
    	foreach ($types as $type => $info) {
    		if ($type == $data['type'] || $info->name == $data['type']) {
    			$found = TRUE;
    			break;
    		}
    	}
    	if (!$found) {
    		return t('Invalid content type: %type', array('%type' => $data['type']));
    	}
    	
    	// check menu
    	$menus = menu_get_menus(TRUE);
    	$found = FALSE;
    	$name = '';
    	foreach ($menus as $name => $title) {
    		if ($title == $data['menu']['parent'] || $name == $data['menu']['parent']) {
    			$found = TRUE;
    			break;
    		}
    	}
    	if (!$found)
    		return t('Invalid menu name: %menu', array('%menu' => $data['menu']['parent']));
    	
    	// check menu item
      $tree = menu_tree_all_data($name);
      $menu_item = _menutrails_patterns_get_menu_item($tree, $data['menu']['item']);
    	if (!$menu_item)
        return t('Invalid menu item: %item', array('%item' => $data['menu']['item']));
    	
    break;

    // Build a patterns actions and parameters
    case 'params':
      return array();
    break;

  }
}


function _menutrails_patterns_get_menu_item(&$tree, $menu_item_name) {
    foreach ($tree as $item) {
      // only want content nodes that are hidden
      if ($menu_item_name == $item['link']['title']) {
      	return $item;
      }
    	if (isset($item['below']) && is_array($item['below']) && !empty($item['below'])) {
        $ret = _menutrails_patterns_get_menu_item($item['below'], $menu_item_name);
        if ($ret) {
        	return $ret;
        }
      }
    }
    return FALSE;
}
