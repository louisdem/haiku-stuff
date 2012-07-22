(require-extension bind coops cplusplus-object)
(bind* #<<EOF
#include <Application.h>
#include "app.h"
EOF
)
(define make-obj (new <AEScan> "application/x-vnd.Haiku-AEScan"))
