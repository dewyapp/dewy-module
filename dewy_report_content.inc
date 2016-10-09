<?php

function dewy_report() {
    // If PHP stops execution at anything under 10 minutes, attempt to set time to 10 minutes
    if (ini_get('max_execution_time') > 0 && ini_get('max_execution_time') < 600) {
        set_time_limit(600);
    }

    // Measure benchmark
    $time_start = microtime(true);

    // Open stream
    $stream = new SplFileObject('php://output', 'w');
    $stream->fwrite('{');

    // Get all text fields on the site
    $text_fields = array();
    $fields = field_info_fields();
    foreach ($fields as $field) {
        if ($field['module'] == 'text') {
            array_push($text_fields, $field);
        }
    }
    unset($fields);

    // Get all node content
    $stream->fwrite('"nodes":{');
    
    $query = db_select('node', 'n');
    $query->fields('n', array('nid'));
    foreach ($text_fields as $field) {
        $table = _field_sql_storage_tablename($field);
        $column = _field_sql_storage_columnname($field['field_name'], 'value');
        $query->leftJoin($table, NULL, "$table.entity_id = n.nid");
        $query->fields(NULL, array($column));
    }
    $result = $query->execute();

    $first_row = TRUE;
    while ($node = $result->fetchAssoc()) {
        if (!$first_row) { 
            $stream->fwrite(',');
        } else {
            $first_row = FALSE;
        }

        $words = 0;
        $content = array();
        $nid = 0;
        foreach ($node as $key => $value) {
            // If key is the nid, store that
            if ($key == 'nid') {
                $nid = $value;
            }
            // Otherwise, the key is a †ext field and the value is content
            else {
                $words = $words + str_word_count($value, 0);
                if ($value) {
                    $content[$key] = $value;
                }
            }
        }

        if (variable_get('dewy_content', 1)) {
            $stream->fwrite(json_encode($nid).':'.json_encode(array('content' => $content, 'words' => $words)));
        }
        else {
            $stream->fwrite(json_encode($nid).':'.json_encode(array('words' => $words)));
        }
    }
    $stream->fwrite('},');
    unset($text_fields);

    // Get all block content
    $stream->fwrite('"blocks":{');
    $result = db_select('block_custom', 'block')
    ->fields('block', array('bid', 'body'))
    ->execute();
    $first_row = TRUE;
    while ($block = $result->fetchAssoc()) {
        if (!$first_row) { 
            $stream->fwrite(',');
        } else {
            $first_row = FALSE;
        }
        if (variable_get('dewy_content', 1)) {
            $stream->fwrite(json_encode($block['bid']).':'.json_encode(array('content' => $block['body'], 'words' => str_word_count($block['body'], 0))));
        }
        else {
            $stream->fwrite(json_encode($block['bid']).':'.json_encode(array('words' => str_word_count($block['body'], 0))));
        }
    }
    $stream->fwrite('},');

    // Report benchmark
    $time_end = microtime(true);
    $time = $time_end - $time_start;
    $stream->fwrite('"benchmark":'.$time);

    // Close stream
    $stream->fwrite('}');
}