% In this ISW simulation, hbar is set to 1

% init simulation parameters
%%
dt = 0.01; % time steps
m = 0.1; % particle mass
L0 = 1; % initial width of square-well
n0 = 1; % initial quantization index
x_c = 0; % center of square-well

L_new = L0; % new square-well width 
f_end = 25; % Fourier expansion cut off term

% domain of simulation
x_min = -5; 
x_max = 5;
steps = 500; % number of steps
dx = (x_max - x_min) / steps; % numerical resolution
x = linspace(x_min, x_max, steps);


%%
% computing simulation wavefunctions

% initial state 
psi_0 = ISW_soln (m, x_c, n0, L0, x, 0);

% set params
fps = 5; % number of frames per seconds 
n_samples = 25 * fps; % total number of frames    

fig = figure;  % open a new figure 
mov = VideoWriter('ISW_simulation.avi');  % set-up the cideo file 
mov.FrameRate = fps; % set the rate of frames 
open(mov);  % open the video file 

% generate frames
for t = 1 : n_samples
    
    % new state
    wavefunctions = [];
    c = [];
    
    % increasing width of square-well
    if (mod(t, 20) == 0)
       L0 = L_new;
       L_new = L_new + 1; 
       % f_end = f_end + 50;
    end
    
    % for loop over linear combination
    for i = 1:f_end
        % coefficients for linear combination
        c_new = coeff (m, x_c, n0, L0, i, L_new, dx, x, t*dt);
        c = [c, c_new];

        % eigenstates for linear combination
        wavefunction = ISW_soln (m, x_c, i, L_new, x, t*dt);
        wavefunctions = [wavefunctions; wavefunction];
    end
    psi = wavefun (c, wavefunctions); % new state
    
    plot4_isw (fig, x_min, x_max, x_c, L_new, x, psi)
    
    drawnow; % updates the figure      
    frame = getframe(fig); % convert the figure into a frame for the video 
    writeVideo(mov,frame); % add frame to video 
    
end 
close(mov); %close the video file 
close(fig); %close the figure  



%%
% defining necessary functions for ISW simulation

function psi = ISW_soln (m, x_c, n, L, x, t)
    %{
    Description:
        Function that computes the eigenstate for given parameters.
    Argumnents:
        - m: particle mass
        - x_c: square-well center
        - n: quantization index
        - L: width of square-well
        - x: x coordinate vector
        - t: current time 
    Return:
        The eigenstate vector.
    %}
    
    phase = exp(- 1j * (n^2 * pi^2) / (2 * m * L) * t);
    psi = sqrt(2/L) * sin((n*pi/L) * (x - x_c + L/2)) * phase;
    
    for i = 1:length(psi)
       if (x(i) < x_c - L/2) || (x(i) > x_c + L/2)
            psi(i) = 0;
       end
    end
end


function c = coeff (m, x_c, n0, L0, n, L, dx, x, t)
    %{
    Description:
        Function that computes the eigenstate coefficient.
    Argumnents:
        - m: particle mass
        - x_c: square-well center
        - n0: initial quantization index
        - L0: initial width of square-well
        - n: new quantization index
        - L: new width of square-well
        - dx: numerical step size
        - x: x coordinate vector
        - t: current time 
    Return:
        Eigenstate coefficient.
    %}

    integral = ISW_soln (m, x_c, n0, L0, x, t) .* ISW_soln (m, x_c, n, L, x, t);
    c = sum(integral) * dx;
end


function psi = wavefun (c_i, wavefunctions)
    %{
    Description:
        Function that computes the linear combination of eigenstate.
    Argumnents:
        - c_i: vector of coefficients
        - wavefunctions: matrix of wavefunctions
    Return:
        The wavefunction vector.
    %}
    
    psi = transpose(wavefunctions) * transpose(c_i);
    psi = transpose(psi); 
end


function plots = plot4_isw (fig, x_min, x_max, x_c, L_new, x, psi)
    %{
    Description:
        Plots wavefunction visualizations on provided figure.
    Argumnents:
        - fig: figure to plot on
        - x_min: x domain boundary (min)
        - x_max: x domain boundary (max)
        - x_c: square-well center
        - L_new: new width of square-well
        - x: x coordinate vector
        - psi: ket (wavefunction) vector
    Return:
        -
    %}

    % plotting full wavefunction
    fig;
    subplot(2, 2, 1);
    plot3(x, imag(psi), real(psi), 'r', 'LineWidth', 2); % plotting new state
    xlabel('x-axis')
    ylabel('$\Im\{\psi(x)\}$', 'Interpreter', 'latex')
    zlabel('$\Re\{\psi(x)\}$', 'Interpreter', 'latex')    
    xlim([x_min x_max])
    ylim([-1.5 1.5])
    zlim([-1.5 1.5])  
    
    % plotting absolute-squared wavefunction
    fig;
    subplot(2, 2, 2)
    p = plot(x, abs(psi).^2, 'k', (x_c - L_new/2)*ones(1, length(x)), linspace(0, 2.25, length(x)), 'k', (x_c + L_new/2)*ones(1, length(x)), linspace(0, 2.25, length(x)), 'k'); % plotting new state
    p(1).LineWidth = 2;
    xlabel('$x$-axis', 'Interpreter', 'latex')
    ylabel('$|\psi(x)|^2$', 'Interpreter', 'latex')    
    xlim([x_min x_max])
    ylim([-0.5 2.25])
    
    % plotting real-part of the wavefunction
    fig;
    subplot(2, 2, 3)
    p1 = plot(x, real(psi), 'b', (x_c - L_new/2)*ones(1, length(x)), linspace(0, 2.25, length(x)), 'k', (x_c + L_new/2)*ones(1, length(x)), linspace(0, 2.25, length(x)), 'k'); % plotting new state
    p1(1).LineWidth = 2;
    xlabel('$x$-axis', 'Interpreter', 'latex')
    ylabel('$\Re\{\psi(x)\}$', 'Interpreter', 'latex')    
    xlim([x_min x_max])
    ylim([-1.5 1.5])
    
    % plotting imaginary-part of the wavefunction
    fig;
    subplot(2, 2, 4)
    p2 = plot(x, imag(psi), (x_c - L_new/2)*ones(1, length(x)), linspace(0, 2.25, length(x)), 'k', (x_c + L_new/2)*ones(1, length(x)), linspace(0, 2.25, length(x)), 'k'); % plotting new state
    p2(1).LineWidth = 2;
    xlabel('$x$-axis', 'Interpreter', 'latex')
    ylabel('$\Im\{\psi(x)\}$', 'Interpreter', 'latex')    
    xlim([x_min x_max])
    ylim([-1.5 1.5])
end





