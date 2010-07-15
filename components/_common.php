<?php
/**
 * Common utility methods for use by Pattern components.
 *
 * @author Ramesh Nair
 */
 

/**
 * Validate 'format'.
 * @return TRUE if validation succeeds; error message string if validation fails.
 */
function _pattern_component_validate_format($data) {
	
      // check that block <format> is valid
      if (isset($data['format'])) {
        $formats = filter_formats();
        // format specified by name?
        if (!is_numeric($data['format'])) {
          foreach ($formats as $id => $format) {
            if ($data['format'] == $format->name)
              return TRUE;
          }
          return t('Invalid input format name: %format', array('%format' => $data['format']));
        } else {
          if (!isset($formats[$data['format']])) {
            return t('Invalid input format id: %format', array('%format' => $data['format']));
          }
        }
      }
  return TRUE;  
}

/**
 * Replace the 'format' name with its id, unless it's already an id.
 * @param $data reference to pattern data. The value of the 'format' key (if set) 
 * will get modified.
 */
function _pattern_component_replace_format_name_with_id(&$data) {
      if (isset($data['format'])) {
        $formats = filter_formats();
        // if format given by name then replace with its id
        if (!is_numeric($data['format'])) {
          foreach ($formats as $id => $format) {
            if ($data['format'] == $format->name) {
              $data['format'] = $id;
              break;              
            }
          }
        }
      }
}


