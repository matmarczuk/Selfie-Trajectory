x1 = linspace(0,10,5);
y1 = [4.9 5.05 5.15 5.05 5];
y2 = y1-2;

xc = linspace(0,10);
y1_= y1;
y2_ = y2;
figure
while (1)
    przeniesienie = y1_(5);
    for i=5:-1:2
        y1_(i)=y1_(i-1);
    end
    y1_(1)=przeniesienie;
    
    y2_= y1_-2;
    p1 = polyfit(x1,y1_,4);
    p2 = polyfit(x1,y2_,4);
    %wyznaczenie punktów przeciecia
    for i=1:4
        punktx1 = [x1(i),x1(i+1)];
        punkty1 = [y1_(i),y2_(i+1)];
        punktx2 = [x1(i),x1(i+1)];
        punkty2 = [y2_(i),y1_(i+1)];
        c1 = polyfit(punktx1,punkty1,1);
        c2 = polyfit(punktx2,punkty2,1);
        
        przeciecie(i) = (c2(2)-c1(2))/(c1(1)-c2(1));
        punkt_przeciecia(i) = polyval(c1,przeciecie(i));
    end
    
    trajectory = polyfit(przeciecie,punkt_przeciecia,3);
    trajectory = polyval(trajectory,xc);
    
    yc1 = polyval(p1,xc);
    yc2 = polyval(p2,xc);
    
    %wyœwietlanie wykresów i trajektorii
    plot(x1,y1_,'o')
    hold on
    plot(x1,y2_,'x')
    hold on
    plot(xc,yc2)
    hold on
    plot(xc,trajectory)
    hold on
    plot(xc,yc1)
    hold on
    plot(przeciecie,punkt_przeciecia,'P');
    hold off
    
    w = waitforbuttonpress;
    if w == 1
        close all;
        break;
    end
end
%zamkniêcie okna

disp('koniec programu')