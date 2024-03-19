; 開啟目前目錄下的 pDescription.txt，依據輸入的關鍵字來篩選說明文字 (txt檔案內，逗號以後的說明文字)
; 呼叫 p_description()
#e::p_description()

chk_file_description()
{
    src := SendCtrlC()
    FileRead, lines, %A_ScriptDir%\pDescription.txt
    Loop, parse, lines, `n, `r
    {
        StringSplit, str, A_LoopField, `, ;以換行符號分離
        if instr(src, str1){
            out .= "`n" A_Loopfield 
        }
    }
    MsgBox % " out=" out
}



get_file_list(key = "", file = "")
{
    FileRead, lines, %file%
    Loop, parse, lines, `n, `r
    {
        StringSplit, str, A_LoopField, `, ;以換行符號分離
        if instr(str2, key){
            out .= "`n" A_Loopfield 
        }
    }
    return out
}

p_description()
{
    ; strCurPath := strGetCurPath()   ;利用WinGetText取得目前路徑
    strDescriptionFile :=  strGetCurPath() "\pDescription.txt"
    if SubStr(strDescriptionFile,1,1) = "\"
    {
        Tip("抓取目錄失敗!", 500)
        return
    }else if(not FileExist(strDescriptionFile)){
        Tip("無描述檔 pDescription.txt", 500)
    }

    strKey := ""
    while(1)
    {
        strList := get_file_list(strKey, strDescriptionFile)
        strShow := "Key: " strKey "`r`n" strList
        MouseGetPos, x, y
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
            strShow := ""
            j := instr(strList, "`r")
            if (strList != ""){
                StringSplit, strName, strList, `, ;以逼號分離
                StringReplace,strTarget,strName1,`n,,,A    ;移除換行
                strPath := strCurPath "\" strTarget
                Send, % strTarget
            }
            break
        }
    }
    ToolTip, 
}

