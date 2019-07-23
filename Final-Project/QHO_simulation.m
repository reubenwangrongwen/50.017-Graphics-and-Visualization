% In this QHO simulation, hbar is set to 1

% init simulation parameters
%%
dt = 0.01;
m = 0.1; % particle mass
omega0 = 5; % initial frequency of harmonic-well
n0 = 0; % initial quantization index
x_c = 0; % center of harmonic-well

omega_new = omega0; % new square-well width 

% domain of simulation
x_min = -10; 
x_max = 10;
steps = 250; % number of steps
dx = (x_max - x_min) / steps; % numerical resolution
x = linspace(x_min, x_max, steps);


%%
% computing simulation wavefunctions

% initial state 
psi_0 = QHO_soln (m, x_c, n0, omega0, x, 0);

% set params
fps = 15; % number of frames per seconds 
n_samples = 10 * fps; % total number of frames    

fig = figure;  % open a new figure 
mov = VideoWriter('QHO_simulation.avi');  % set-up the cideo file 
mov.FrameRate = fps; % set the rate of frames 
open(mov);  % open the video file 

% generate frames
for t = 1 : n_samples
    
    % increasing width of square-well
    if (mod(t, 20) == 0)
       omega0 = omega_new; 
       omega_new = omega_new - 0.5; 
    end
    
    lambda = omega_new / omega0; % potential scale factor
    psi = QHO_ket0 (m, x_c, omega0, lambda, x, t*dt);
    
    plot4_qho (fig, x_min, x_max, m, omega_new, x, psi)
    
    drawnow; % updates the figure      
    frame = getframe(fig); % convert the figure into a frame for the video 
    writeVideo(mov,frame); % add frame to video 
    
end 
close(mov); %close the video file 
close(fig); %close the figure  



%%
% defining necessary functions for ISW simulation
function psi = QHO_ket0 (m, x_c, omega0, lambda, x, t)
    %{
    Description:
        Function that computes the QHO ground state for given parameters.
    Argumnents:
        - m: particle mass
        - x_c: square-well center
        - omega0: width of original harmonic-well
        - lambda: harmonic-well scale factor
        - x: x coordinate vector
        - t: current time 
    Return:
        The groundstate vector.
    %}

    arg = lambda*omega0*t; % sinusoid argument
    phase = 1 / sqrt(cos(arg) + 1j/lambda*sin(arg)); % complex phase
    phi = (1 + 1j*lambda*tan(arg)) / (1 + 1j/lambda * tan(arg));
    f = (m * omega0 / pi)^(1/4); % coefficient factor
    
    psi = f * exp(- phi * (m * omega0 / 2) * (x - x_c).^2) * phase; % state
end


function psi = QHO_soln (m, x_c, n, omega, x, t)
    %{
    Description:
        Function that computes the QHO eigenstate for given parameters.
    Argumnents:
        - m: particle mass
        - x_c: square-well center
        - n: quantization index
        - omega0: width of square-well
        - x: x coordinate vector
        - t: current time 
    Return:
        The eigenstate vector.
    %}

    f = (m * omega / pi)^(1/4) / sqrt(2^n * factorial(n)); % front factor
    H_n = hermiteH(n, sqrt(m*omega) * (x - x_c)); % Hermite polynomial
    phase = exp(- 1j * omega * t); % complex phase
    
    psi = f * exp(- m * omega * (x - x_c).^2 / 2) .* H_n * phase; % state
end


function plots = plot4_qho (fig, x_min, x_max, m, omega_new, x, psi)
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

    fig;
    % plotting full wavefunction
    subplot(2,2,1)
    p = plot3(x, real(psi), imag(psi), 'r');
    p(1).LineWidth = 2;
    xlabel('$x$-axis', 'Interpreter', 'latex')
    ylabel('$\Re\psi(x)$', 'Interpreter', 'latex')
    zlabel('$\Im\psi(x)$', 'Interpreter', 'latex')
    xlim([x_min x_max])
    ylim([-2 2])
    zlim([-2 2])
    
    subplot(2,2,2)
    p1 = plot(x, abs(psi), 'm', x, 0.5 * m * omega_new^2 * x.^2, 'k');
    p1(1).LineWidth = 2;
    xlabel('$x$-axis', 'Interpreter', 'latex')
    ylabel('$|\psi(x)|$', 'Interpreter', 'latex')  
    xlim([x_min x_max])
    ylim([-2 2])
    
    subplot(2,2,3)
    p2 = plot(x, real(psi), x, 0.5 * m * omega_new^2 * x.^2, 'k');
    p2(1).LineWidth = 2;
    xlabel('$x$-axis', 'Interpreter', 'latex')
    ylabel('$\Re\psi(x)$', 'Interpreter', 'latex')  
    xlim([x_min x_max])
    ylim([-2 2])
    
    subplot(2,2,4)
    p3 = plot(x, imag(psi), 'b', x, 0.5 * m * omega_new^2 * x.^2, 'k');
    p3(1).LineWidth = 2;
    xlabel('$x$-axis', 'Interpreter', 'latex')
    ylabel('$\Im\psi(x)$', 'Interpreter', 'latex')  
    xlim([x_min x_max])
    ylim([-2 2])
end





