; 回傳有包含關鍵字的字串
; strList 為多行字串
list_filtered_by_key(strList = "", key = ""){
    Loop, parse, strList, `n, `r
    {
        if (A_LoopField = "")  ; Omit the last linefeed (blank item) at the end of the list.
            continue
        if instr(A_Loopfield, key){
            out .= "`n" A_Loopfield 
        }
    }
    return out
}

; 取出多行字串中的某個字串
; strList 為多行字串，以換行符號分隔 `n
; n_req   目標字串編號 (由 0 開始)
list_get_n(strList, n_req){
    n := 0
    Loop, parse, strList, `n, `r
    {
        if (A_LoopField = "")  ; Omit the last linefeed (blank item) at the end of the list.
            continue
        if (n = n_req)
            return A_LoopField
        n := n + 1
    }
    return ""
}

; 在多行字串中的每個字串加上編號 (如下)，由 0 開始，9 之後接上英文字母 0, 1, 2, ...9, a, b, c, ...
;   0. xxx
;   1. xxx
; strList 為多行字串，以換行符號分隔 `n
add_num(strList){
    n := 0
    Loop, parse, strList, `n, `r
    {
        if (A_LoopField = "")  ; Omit the last linefeed (blank item) at the end of the list.
            continue
        str_out .= "`n" n2alpha(n) ". " A_Loopfield
        n := n + 1
    }
    return str_out
}

; 給定多行字串，讓使用者輸入關鍵字過濾後，列出編號讓使用者選取
p_filter_select(strList){
    strKey := ""
    while(1)
    {
        str_filtered_list := list_filtered_by_key(strList, strKey)
        strShow := "Key: " strKey "`r`n" str_filtered_list
        x := A_ScreenWidth / 2
        y := A_ScreenHeight * 1 / 4
        ToolTip, %strShow%, x, y    ; 顯示提示文字

        Input, ch, L1, {BackSpace}
        FoundPos := RegExMatch(ErrorLevel, "EndKey:(.*)", SubPat)   ; 偵測 Backspace
        if (FoundPos > 0)
            ch := SubPat1

        if(ch = "Backspace"){ ; Backspace
            if(StrLen(strKey) > 0){
                strKey := SubStr(strKey, 1, StrLen(strKey)-1)
            }
        }else{
            strKey .= ch
        }

        if(asc(ch) = 27){
            break
        }else if(asc(ch) = 10){
            Tooltip
            r := p_select(str_filtered_list)
            return r
            ; strShow := ""
            ; if (str_filtered_list != ""){
            ;     strNumList := add_num(str_filtered_list)
            ;     c := menu_select("Select:`n" strNumList)
            ;     r := list_get_n(str_filtered_list, c)
            ;     return r
            ; }
            break
        }
    }
    ToolTip, 
}

p_select(strList){
    if (strList != ""){
        strNumList := add_num(strList)
        c := menu_select("Select:`n" strNumList)
        r := list_get_n(strList, c)
        return r
    }
}

