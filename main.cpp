#include "sphere.h"

int main() {
    Window main_window(1400, 800, TX_WHITE);
    Texture gas_can(600, 600, TX_WHITE, 0, 0);

    Coordinates coord(0, 10, 0, 10);
    Renderer render(&gas_can, &coord);
    Manager manager(1000);

    for (int i = 0; i < 5; ++i) {
        PhysSphere* tmp = new PhysSphere(Random(0.5, 9.5), Random(0.5, 9.5), Random(0.2, 0.4), Random(1, 5), Random(1, 5), 1, RGB(128, Random(0, 255), Random(0, 255)),
                                         RGB(20, 100, 40), RGB(0, Random(0, 255), 150), vec3d(li_x, li_y, li_z), vec3d(vi_x, vi_y, vi_z));
        manager.addFigure(tmp);
    }
    for (int i = 0; i < 3; ++i) {
        double x = Random(0.5, 9.5);
        double y = Random(0.5, 9.5);
        PhysRect* tmp = new PhysRect(x, y, x + Random(0.2, 0.4), y + Random(0.2, 0.4), Random(1, 5), Random(1, 5), 1,
                                     RGB(Random(0, 255), Random(0, 255), Random(0, 255)));
        manager.addFigure(tmp);
    }

    Texture texture_graph(400, 180, TX_WHITE, 10, 600);
    Graph graph(10, 0);
    Coordinates graph_coord(0, 10, 0, 30);
    Renderer graph_render(&texture_graph, &graph_coord);
    graph.addDot(manager.getCount());
    graph.drawAxis(graph_render);
    graph.draw(graph_render);
    texture_graph.show_on(&main_window);

    Coordinates button_coord(0, 800, 0, 750);
    Texture button_texture(800, 750, TX_WHITE, 600, 0);
    Renderer button_render(&button_texture, &button_coord);

    AddCircle functor_circle(&manager);
    AddRect functor_rect(&manager);
    IncreaseTemp functor_inc(&manager);
    DecreaseTemp functor_dec(&manager);
    AdjustScale functor_scale(&manager, &graph_render);
    TextButton* button1 = new TextButton(200, 80, 100, 650, TX_BLUE, "Add Circle!", &functor_circle);
    TextButton* button2 = new TextButton(200, 80, 100, 500, TX_BLUE, "Add Rectangle!", &functor_rect);
    TextButton* button3 = new TextButton(200, 80, 100, 350, TX_BLUE, "Inc Temperature!", &functor_inc);
    TextButton* button4 = new TextButton(200, 80, 100, 200, TX_BLUE, "Dec Temperature!", &functor_dec);
    TextButton* button5 = new TextButton(200, 80, 100, 50, TX_BLUE, "Adjust Scale!", &functor_scale);

    ButtonManager button_manager(10);
    button_manager.addButton(button1);
    button_manager.addButton(button2);
    button_manager.addButton(button3);
    button_manager.addButton(button4);
    button_manager.addButton(button5);


    int obj_count = manager.getCount();
    double time = dt;
    while (!GetAsyncKeyState(VK_ESCAPE)) {
        render.clear();
        render.drawRectangle(0, 0, 10, 10, TX_BLACK, 2);
        manager.drawAll(render);
        manager.moveAll(time);
        CheckAllCollisions(manager, render.getCoordinates());
        gas_can.show_on(&main_window);

        button_manager.drawAll(button_render);
        button_manager.checkAll(button_render, manager);        
        button_texture.show_on(&main_window);

        //if (obj_count != manager.getCount()) {
            obj_count = manager.getCount();
            graph.addDot(obj_count);
            graph_render.clear();
            graph.drawAxis(graph_render);
            graph.draw(graph_render);
            texture_graph.show_on(&main_window);
        //}

        txSleep(30);
    }
    
    return 0;
}