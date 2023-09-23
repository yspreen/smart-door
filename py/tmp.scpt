tell application "Keychain Scripting" to tell keychain "login.keychain"

    repeat with x from 1 to (count every Internet key)
        if name of Internet key x contains "Google" then
            return {account, password} of Internet key x
        end if

    end repeat
end tell