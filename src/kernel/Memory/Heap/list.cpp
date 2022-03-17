#include <mapple/Memory.h>

void add_node(bin_t *bin, node_t* node) {
    node->next = nullptr;
    node->prev = nullptr;

    node_t *temp = bin->head;

    if (bin->head == nullptr) {
        bin->head = node;
        return;
    }
    
    // we need to save next and prev while we iterate
    node_t *current = bin->head;
    node_t *previous = nullptr;
    // iterate until we get the the end of the list or we find a 
    // node whose size is
    while (current != nullptr && current->size <= node->size) {
        previous = current;
        current = current->next;
    }

    if (current == nullptr) { // we reached the end of the list
        previous->next = node;
        node->prev = previous;
    }
    else {
        if (previous != nullptr) { // middle of list, connect all links!
            node->next = current;
            previous->next = node;

            node->prev = previous;
            current->prev = node;
        }
        else { // head is the only element
            node->next = bin->head;
            bin->head->prev = node;
            bin->head = node;
        }
    }
}

void remove_node(bin_t * bin, node_t *node) {
    if (bin->head == nullptr) return; 
    if (bin->head == node) { 
        bin->head = bin->head->next;
        return;
    }
    
    node_t *temp = bin->head->next;
    while (temp != nullptr) {
        if (temp == node) { // found the node
            if (temp->next == nullptr) { // last item
                temp->prev->next = nullptr;
            }
            else { // middle item
                temp->prev->next = temp->next;
                temp->next->prev = temp->prev;
            }
            // we dont worry about deleting the head here because we already checked that
            return;
        }
        temp = temp->next;
    }
}

node_t *get_best_fit(bin_t *bin, size_t size) {
    if (bin->head == nullptr) return nullptr; // empty list!

    node_t *temp = bin->head;

    while (temp != nullptr) {
        if (temp->size >= size) {
            return temp; // found a fit!
        }
        temp = temp->next;
    }
    return nullptr; // no fit!
}

node_t *get_last_node(bin_t *bin) {
    node_t *temp = bin->head;

    while (temp->next != nullptr) {
        temp = temp->next;
    }
    return temp;
}
