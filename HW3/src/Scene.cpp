/**************************************************
Scene.cpp contains the implementation of the draw command
*****************************************************/
#include "Scene.h"
#include "Cube.h"
#include "Obj.h"

// The scene init definition 
#include "Scene.inl"


using namespace glm;
//void Scene::draw(void){
//    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
//    camera -> computeMatrices();
//    shader -> view = camera -> view;
//    shader -> projection = camera -> proj;
//    shader -> nlights = light.size();
//    shader -> lightpositions.resize( shader -> nlights );
//    shader -> lightcolors.resize( shader -> nlights );
//    int count = 0;
//    for ( std::pair<std::string,Light*> entry : light ){
//        shader -> lightpositions[ count ] = (entry.second) -> position;
//        shader -> lightcolors[ count ] = (entry.second) -> color;
//        count++;
//    }
//    
//    // Define stacks for depth-first search (DFS)
//    std::stack < Node* > dfs_stack;
//    std::stack < mat4 >  matrix_stack; // HW3: You will update this matrix_stack during the depth-first search while loop.
//    
//    // Initialize the current state variable for DFS
//    Node* cur = node["world"]; // root of the tree
//    mat4 cur_VM = camera -> view; // HW3: You will update this current modelview during the depth first search.  Initially, we are at the "world" node, whose modelview matrix is just camera's view matrix.
//    
//    // HW3: The following is the beginning of the depth-first search algorithm.
//    // HW3: The depth-first search for the node traversal has already been implemented (cur, dfs_stack).
//    // HW3: All you have to do is to also update the states of (cur_VM, matrix_stack) alongside the traversal.  You will only need to modify starting from this line.
//    dfs_stack.push( cur );
//    matrix_stack.push(cur_VM);
//    
//    while( ! dfs_stack.empty() ){
//        // Detect whether the search runs into infinite loop by checking whether the stack is longer than the size of the graph.
//        // Note that, at any time, the stack does not contain repeated element.
//        /*if (dfs_stack.size() > node.size() ){
//            std::cerr << "Error: The scene graph has a closed loop." << std::endl;
//            exit(-1);
//        }*/
//        
//        // top-pop the stacks
//        cur = dfs_stack.top();        dfs_stack.pop();
//        // top-pop the view matrix
//        cur_VM = matrix_stack.top();
//        matrix_stack.pop();
//        
//        // draw all the models at the current node
//        for ( unsigned int i = 0; i < cur -> models.size(); i++ ){
//            // Prepare to draw the geometry. Assign the modelview and the material.
//            
//            // compute the new view matrix
//            mat4 new_VM = cur_VM * (cur->modeltransforms[i]);
//
//            shader -> modelview = new_VM; // HW3: Without updating cur_VM, modelview would just be camera's view matrix.
//            shader -> material  = ( cur -> models[i] ) -> material;
//            
//            
//            
//            // The draw command
//            shader -> setUniforms();
//            ( cur -> models[i] ) -> geometry -> draw();
//        }
//        
//        // Continue the DFS: put all the child nodes of the current node in the stack
//        for ( unsigned int i = 0; i < cur -> childnodes.size(); i++ ){
//            dfs_stack.push( cur -> childnodes[i] );
//            // compute and push child view matrices
//            mat4 new_child_VM = cur_VM * (cur->childtransforms[i]);
//            matrix_stack.push(new_child_VM);
//        }
//        
//    } // End of DFS while loop.
//    // HW3: Your code will only be above this line.
//    
//}

void Scene::draw(SurfaceShader* shader) {
    glUseProgram(shader -> program);
    // camera-space view, proj set already to view, projection
    // set light-space equivalents
    
    shader -> view_light = light["sun"] -> view;
    shader -> projection_light = light["sun"] -> proj;

    # pragma region pre-draw_sequence
    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
    camera->computeMatrices();
    shader->view = camera->view;
    shader->projection = camera->proj;
    shader->nlights = light.size();
    shader->lightpositions.resize(shader->nlights);
    shader->lightcolors.resize(shader->nlights);
    int count = 0;
    for (std::pair<std::string, Light*> entry : light) {
        shader->lightpositions[count] = (entry.second)->position;
        shader->lightcolors[count] = (entry.second)->color;
        count++;
    }
    # pragma endregion

    // Define stacks for depth-first search (DFS)
    std::stack < Node* > dfs_stack;
    std::stack < mat4 >  matrix_stack;

    // Initialize the current state variable for DFS
    Node* cur = node["world"]; // root of the tree

    // we want model matrix independent of view
    mat4 cur_M = glm::mat4(1.0f); // identity matrix

    // HW3: The following is the beginning of the depth-first search algorithm.
    // HW3: The depth-first search for the node traversal has already been implemented (cur, dfs_stack).
    // HW3: All you have to do is to also update the states of (cur_VM, matrix_stack) alongside the traversal.  You will only need to modify starting from this line.
    dfs_stack.push(cur);
    matrix_stack.push(cur_M);

    while (!dfs_stack.empty()) {
        // Detect whether the search runs into infinite loop by checking whether the stack is longer than the size of the graph.
        // Note that, at any time, the stack does not contain repeated element.

        // top-pop the stacks
        cur = dfs_stack.top();        dfs_stack.pop();
        // top-pop the view matrix
        cur_M = matrix_stack.top();
        matrix_stack.pop();

        // draw all the models at the current node
        for (unsigned int i = 0; i < cur->models.size(); i++) {
            // Prepare to draw the geometry. Assign the model and the material.

            // compute the new model matrix
            mat4 new_M = cur_M * (cur->modeltransforms[i]);

            shader->model = new_M;
            shader->material = (cur->models[i])->material;



            // The draw command
            shader->setUniforms();
            (cur->models[i])->geometry->draw();
        }

        // Continue the DFS: put all the child nodes of the current node in the stack
        for (unsigned int i = 0; i < cur->childnodes.size(); i++) {
            dfs_stack.push(cur->childnodes[i]);
            // compute and push child view matrices
            mat4 new_child_M = cur_M * (cur->childtransforms[i]);
            matrix_stack.push(new_child_M);
        }

    } // End of DFS while loop.

}

void Scene::draw(DepthShader* shader) {
    glUseProgram(shader->program);
    // camera-space view, proj set already to view, projection
    // set light-space equivalents

    // get the camera info into the light for psm
    camera->computeMatrices();
    light["sun"]->camera = camera;

    light["sun"]->computeMatrices();
    shader->view = light["sun"]->view;
    shader->projection = light["sun"]->proj;

# pragma region pre-draw_sequence
    // Pre-draw sequence: assign uniforms that are the same for all Geometry::draw call.  These uniforms include the camera view, proj, and the lights.  These uniform do not include modelview and material parameters.
# pragma endregion

    // Define stacks for depth-first search (DFS)
    std::stack < Node* > dfs_stack;
    std::stack < mat4 >  matrix_stack;

    // Initialize the current state variable for DFS
    Node* cur = node["world"]; // root of the tree

    // we want model matrix independent of view
    mat4 cur_M = glm::mat4(1.0f); // identity matrix

    // HW3: The following is the beginning of the depth-first search algorithm.
    // HW3: The depth-first search for the node traversal has already been implemented (cur, dfs_stack).
    // HW3: All you have to do is to also update the states of (cur_VM, matrix_stack) alongside the traversal.  You will only need to modify starting from this line.
    dfs_stack.push(cur);
    matrix_stack.push(cur_M);

    while (!dfs_stack.empty()) {
        // Detect whether the search runs into infinite loop by checking whether the stack is longer than the size of the graph.
        // Note that, at any time, the stack does not contain repeated element.

        // top-pop the stacks
        cur = dfs_stack.top();        dfs_stack.pop();
        // top-pop the view matrix
        cur_M = matrix_stack.top();
        matrix_stack.pop();

        // draw all the models at the current node
        for (unsigned int i = 0; i < cur->models.size(); i++) {
            // Prepare to draw the geometry. Assign the model and the material.

            // compute the new model matrix
            mat4 new_M = cur_M * (cur->modeltransforms[i]);

            shader->model = new_M;

            // The draw command
            shader->setUniforms();
            (cur->models[i])->geometry->draw();
        }

        // Continue the DFS: put all the child nodes of the current node in the stack
        for (unsigned int i = 0; i < cur->childnodes.size(); i++) {
            dfs_stack.push(cur->childnodes[i]);
            // compute and push child view matrices
            mat4 new_child_M = cur_M * (cur->childtransforms[i]);
            matrix_stack.push(new_child_M);
        }

    } // End of DFS while loop.

}