/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ppajot <ppajot@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/25 20:24:04 by ppajot            #+#    #+#             */
/*   Updated: 2022/06/27 20:11:28 by ppajot           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../philo.h"

void	display(long t_zero, int id, pthread_mutex_t *displaymutex, char *str)
{
	struct timeval tv;

	gettimeofday(&tv, 0);
	pthread_mutex_lock(displaymutex);
	printf("%li %i %s\n", tv.tv_sec * 1000 + tv.tv_usec / 1000 - t_zero, id, str);
	pthread_mutex_unlock(displaymutex);
}

void	*philo(void *arg)
{
	t_data *data = (t_data *)arg;
	struct timeval tv;
	int		eat_sleep_think = 0;
	int		id;
	
	pthread_mutex_lock(&data->idmutex);	
	id = ++data->philid;
	pthread_mutex_unlock(&data->idmutex);
	pthread_mutex_lock(&data->endmutex);
	while (data->end)
	{
		pthread_mutex_unlock(&data->endmutex);
		if (eat_sleep_think == 3)
		{
			display(data->t_zero, id, &data->display, "is thinking");
			eat_sleep_think = 0;
		}
		if (eat_sleep_think == 2)
		{
			display(data->t_zero, id, &data->display, "is sleeping");
			usleep(data->timetosleep * 1000);
			eat_sleep_think = 3;
		}
		pthread_mutex_lock(&data->forkmutex[((id / 2) * 2) % data->philnbr]);
		if (data->forktab[((id / 2) * 2) % data->philnbr] && eat_sleep_think == 1)
		{			
			data->forktab[((id / 2) * 2) % data->philnbr] = 0;
			pthread_mutex_unlock(&data->forkmutex[((id / 2) * 2) % data->philnbr]);
			display(data->t_zero, id, &data->display, "has taken a fork");
			gettimeofday(&tv, 0);
			pthread_mutex_lock(&data->lasteatmutex);
			data->lasteat[id - 1] = tv.tv_sec * 1000 + tv.tv_usec / 1000;
			pthread_mutex_unlock(&data->lasteatmutex);
			display(data->t_zero, id, &data->display, "is eating");
			pthread_mutex_lock(&data->meals);
			data->mealcount[id - 1]++;
			pthread_mutex_unlock(&data->meals);
			usleep(data->timetoeat * 1000);
			pthread_mutex_lock(&data->forkmutex[(((id - 1) / 2) * 2 + 1) % data->philnbr]);
			pthread_mutex_lock(&data->forkmutex[((id / 2) * 2) % data->philnbr]);
			data->forktab[(((id - 1) / 2) * 2 + 1) % data->philnbr] = 1;
			data->forktab[((id / 2) * 2) % data->philnbr] = 1;
			pthread_mutex_unlock(&data->forkmutex[(((id - 1) / 2) * 2 + 1) % data->philnbr]);
			eat_sleep_think = 2;
		}
		pthread_mutex_unlock(&data->forkmutex[((id / 2) * 2) % data->philnbr]);
		pthread_mutex_lock(&data->forkmutex[(((id - 1) / 2) * 2 + 1) % data->philnbr] );
		if (data->forktab[(((id - 1) / 2) * 2 + 1) % data->philnbr] && eat_sleep_think == 0)
		{			
			data->forktab[(((id - 1) / 2) * 2 + 1) % data->philnbr] = 0;
			display(data->t_zero, id, &data->display, "has taken a fork");
			eat_sleep_think = 1;
		}
		pthread_mutex_unlock(&data->forkmutex[(((id - 1) / 2) * 2 + 1) % data->philnbr]);	
		pthread_mutex_lock(&data->endmutex);
	}
	pthread_mutex_unlock(&data->endmutex);
	return (0);
}

int	check_end(void *arg)
{
	t_data *data = (t_data *)arg;
	struct timeval tv;
	int	i;
	int counter;
	
	usleep(100);
	while (1)
	{
		i = -1;
		counter = 0;
		while (++i < data->philnbr)
		{
			pthread_mutex_lock(&data->meals);
			if (data->mealcount[i] >= data->mealnbr)
				counter++;
			pthread_mutex_unlock(&data->meals);
			if (counter == data->philnbr)
			{
				pthread_mutex_lock(&data->endmutex);
				data->end = 0;
				pthread_mutex_unlock(&data->endmutex);
				return (0);
			}
			pthread_mutex_lock(&data->lasteatmutex);
			gettimeofday(&tv, 0);
			if (tv.tv_sec * 1000 + tv.tv_usec / 1000 - data->lasteat[i] > data->timetodie)
			{
				pthread_mutex_lock(&data->display);
				printf("%li %i died\n", tv.tv_sec * 1000 + tv.tv_usec / 1000 - data->t_zero, i + 1);
				pthread_mutex_lock(&data->endmutex);
				data->end = 0;
				pthread_mutex_unlock(&data->endmutex);
				pthread_mutex_unlock(&data->display);
				pthread_mutex_unlock(&data->lasteatmutex);
				return (0);
			}
			pthread_mutex_unlock(&data->lasteatmutex);
		}
	}
	return (0);
}

int	ft_atoi(char *str)
{
	int	res;

	res = 0;
	while (*str)
	{
		res = *str - 48 + res * 10;
		str++;
	}
	return (res);
}

int	check_arg(char *str)
{
	int	i;
	
	i = -1;
	while (str[++i])
		if (str[i] < '0' || str[i] > '9')
			return (0);
	return (1);
}

int	check_av(int ac, char **av)
{
	int	i;

	i = 0;
	if (ac < 5 || ac > 6)
		return (0);
	while (++i < ac)
	{
		if (!check_arg(av[i]))
			return (0);
		if (i == 1 && !ft_atoi(av[i]))
			return (0);	
	}
	return (1);
}

t_data	init_data(int ac, char **av)
{
	t_data data;
	struct timeval tv;

	gettimeofday(&tv, 0);
	data.t_zero = tv.tv_sec * 1000 + tv.tv_usec / 1000;
	data.end = 1;
	data.philnbr = ft_atoi(av[1]);
	data.timetodie = ft_atoi(av[2]);
	data.timetoeat = ft_atoi(av[3]);
	data.timetosleep = ft_atoi(av[4]);
	if (ac == 6)
		data.mealnbr = ft_atoi(av[5]);
	else
		data.mealnbr = 2147483647;
	data.forktab = (int *)malloc(sizeof(int) * data.philnbr);
	data.mealcount = (int *)malloc(sizeof(int) * data.philnbr);
	data.tid = (pthread_t *)malloc(sizeof(pthread_t) * data.philnbr);
	data.lasteat = (long *)malloc(sizeof(long) * data.philnbr);
	data.forkmutex = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t) * data.philnbr);
	int	i = -1;
	while (++i < data.philnbr)
	{
		data.mealcount[i] = 0;
		data.forktab[i] = 1;
		pthread_mutex_init(&data.forkmutex[i], 0);
		data.lasteat[i] = data.t_zero;
	}
	return (data);
}		

int	main(int ac, char **av)
{
	t_data data;
	data = init_data(ac, av);
	pthread_mutex_init(&data.display, 0);
	pthread_mutex_init(&data.lasteatmutex, 0);
	pthread_mutex_init(&data.idmutex, 0);
	pthread_mutex_init(&data.endmutex, 0);
	pthread_mutex_init(&data.meals, 0);
	int	i = 0;
	data.philid = 0;
	while (i < data.philnbr)
	{			
		pthread_create(&data.tid[i], NULL, philo, &data);
		usleep(10);
		i++;
	}
	if (check_end(&data) == 0)
	{
		i = -1;
		while (++i < data.philnbr)
			pthread_join(data.tid[i], NULL);
		free(data.lasteat);
		return (0);	
	}
	return (0);
}