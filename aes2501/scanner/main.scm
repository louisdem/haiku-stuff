(require-extension bind coops cplusplus-object)
(bind* "#include <Application.h>\n#include \"app.h\"\n#include \"common\/aes2501_app_common.h\"")
; (define instance (force init-val))
(define init-val (delay
 (let ((obj (new <AEScan> kAesSignature)))
  (_Run obj)
  obj
 )
))
(define (do-exit obj)
 (delete (DoQuit obj))
)

;;;
