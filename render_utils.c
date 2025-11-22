/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <render_utils.h>

void delete_slivers(Rectangle oldRect, Rectangle newRect){
    if (oldRect.x == -1) return;
    
    if (newRect.x == -1) {
        oledc_rectangle(oldRect.x, oldRect.y, 
                        oldRect.x + oldRect.width, 
                        oldRect.y + oldRect.height, 0x0000);
        return;
    }

    CPU_INT08S old_x2 = oldRect.x + oldRect.width;
    CPU_INT08S old_y2 = oldRect.y + oldRect.height;
    CPU_INT08S new_x2 = newRect.x + newRect.width;
    CPU_INT08S new_y2 = newRect.y + newRect.height;

    if (newRect.x > oldRect.x) {
        oledc_rectangle(oldRect.x, oldRect.y, newRect.x, old_y2, 0x0000);
    }
    
    
    if (new_x2 < old_x2) {
        
        oledc_rectangle(new_x2, oldRect.y, old_x2, old_y2, 0x0000);
    }

    if (newRect.y > oldRect.y) {
        oledc_rectangle(oldRect.x, oldRect.y, old_x2, newRect.y, 0x0000);
    }

    
    if (new_y2 < old_y2) {
        oledc_rectangle(oldRect.x, new_y2, old_x2, old_y2, 0x0000);
    }
}




/* [] END OF FILE */
