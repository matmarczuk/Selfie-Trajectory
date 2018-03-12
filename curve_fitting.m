left_x = linspace(0,10,5);
left_y = [4.9 5.05 5.15 5.05 5]; %przebieg testowy punktow
right_y = left_y-2;

x_axis = linspace(0,10);
sim_left_y= left_y;
sim_right_y = right_y;
figure
while (1)
    overflow = sim_left_y(5);
    for i=5:-1:2
        sim_left_y(i)=sim_left_y(i-1);
    end
    sim_left_y(1)=overflow;
    
    sim_right_y= sim_left_y-2;
    poly_left = polyfit(left_x,sim_left_y,4);
    poly_right = polyfit(left_x,sim_right_y,4);
    %wyznaczenie punkt�w przeciecia
    for i=1:4
        cross_point_x= [left_x(i),left_x(i+1)];
        cross_point_y1 = [sim_left_y(i),sim_right_y(i+1)];
        cross_point_y2 = [sim_right_y(i),sim_left_y(i+1)];
        linear1 = polyfit(cross_point_x,cross_point_y1,1);
        linear2 = polyfit(cross_point_x,cross_point_y2,1);
        
        intersec_x(i) = (linear2(2)-linear1(2))/(linear1(1)-linear2(1));
        intersec_y(i) = polyval(linear1,intersec_x(i));
    end
    
    trajectory = polyfit(intersec_x,intersec_y,3);
    trajectory = polyval(trajectory,x_axis);
    
    poly_left = polyval(poly_left,x_axis);
    poly_right = polyval(poly_right,x_axis);
    
    %wy�wietlanie wykres�w i trajektorii
    plot(left_x,sim_left_y,'o')
    hold on
    plot(left_x,sim_right_y,'x')
    hold on
    plot(x_axis,poly_right)
    hold on
    plot(x_axis,trajectory)
    hold on
    plot(x_axis,poly_left)
    hold on
    plot(intersec_x,intersec_y,'P');
    hold off
    
    w = waitforbuttonpress;
    if w == 1
        close all;
        break;
    end
end
%zamkni�cie okna

disp('koniec programu')