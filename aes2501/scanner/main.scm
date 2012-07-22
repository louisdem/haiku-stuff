(require-extension bind coops cplusplus-object)
(bind* #<<EOF
#include <Application.h>
#include "app.h"
EOF
)
(define init-val (delay
 (let ((obj (new <AEScan> "application/x-vnd.Haiku-AEScan")))
  (_Run obj)
  obj
 )
))

;;;
